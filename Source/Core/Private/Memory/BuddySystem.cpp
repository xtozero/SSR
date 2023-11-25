#include "BuddySystem.h"

#include <algorithm>
#include <cassert>
#include <cmath>

namespace
{
	template <typename T>
	T GetNearestPowerOfTwo( T x )
	{
		return static_cast<T>( 1 ) << static_cast<T>( std::ceil( std::log2( x ) ) );
	}
}

bool BuddySystem::Initialize( size_t size, size_t minBlockSize ) noexcept
{
	if ( size == 0 || minBlockSize == 0 )
	{
		return false;
	}

	m_size = GetNearestPowerOfTwo( size );
	if ( m_size > size )
	{
		m_size >>= 1;
	}

	m_minBlockSize = GetNearestPowerOfTwo( minBlockSize );

	if ( m_size < m_minBlockSize )
	{
		return false;
	}

	m_numLevel = std::log2( m_size / m_minBlockSize ) + 1;

	m_freeFlags.resize( static_cast<size_t>( 1 ) << m_numLevel );
	m_freeFlags[1] = true;

	m_usedBlockLevels.resize( m_size / m_minBlockSize, -1 );

	return true;
}

std::optional<size_t> BuddySystem::Allocate( size_t size )
{
	uint32 level = GetLevel( size );

	std::optional<size_t> blockIdx = GetFreeBlock( level );
	if ( blockIdx.has_value() == false )
	{
		for ( int curLevel = level + 1; curLevel < m_numLevel; ++curLevel )
		{
			blockIdx = GetFreeBlock( curLevel );
			if ( blockIdx.has_value() )
			{
				blockIdx = SplitBlock( curLevel, level, blockIdx.value() );
				break;
			}
		}
	}

	if ( blockIdx.has_value() == false )
	{
		return {};
	}

	SetBlockState( level, blockIdx.value(), false );

	size_t blockSize = m_minBlockSize << level;
	size_t offset = blockIdx.value() * blockSize;

	m_usedSize += blockSize;
	m_usedBlockLevels[offset / m_minBlockSize] = level;

	return offset;
}

void BuddySystem::Deallocate( size_t offset )
{
	int32 level = m_usedBlockLevels[offset / m_minBlockSize];

	if ( level == -1 )
	{
		assert( false && "Trying to release unallocated space" );
		return;
	}

	m_usedBlockLevels[offset / m_minBlockSize] = -1;

	size_t blockSize = m_minBlockSize << level;
	size_t blockIndex = offset / blockSize;

	SetBlockState( level, blockIndex, true );
	MergeBlcok( blockIndex, level );

	m_usedSize -= blockSize;
}

size_t BuddySystem::Capacity() const noexcept
{
	return m_size;
}

size_t BuddySystem::UsedSize() const noexcept
{
	return m_usedSize;
}

size_t BuddySystem::AvailableSize() const noexcept
{
	return Capacity() - UsedSize();
}

size_t BuddySystem::GetSuitableBlockSize( size_t size ) const noexcept
{
	size = GetNearestPowerOfTwo( size );
	return std::max<size_t>( size, m_minBlockSize );
}

uint32 BuddySystem::GetLevel( size_t size ) const noexcept
{
	size = GetSuitableBlockSize( size );
	return std::log2( size / m_minBlockSize );
}

size_t BuddySystem::GetBlockStateOffset( uint32 level ) const noexcept
{
	return static_cast<size_t>( 1 ) << ( m_numLevel - level - 1 );
}

std::optional<size_t> BuddySystem::GetFreeBlock( uint32 level ) const noexcept
{
	size_t offset = GetBlockStateOffset( level );
	size_t numBlock = ( m_size / m_minBlockSize ) >> level;

	for ( size_t i = 0; i < numBlock; ++i )
	{
		if ( m_freeFlags[offset + i] )
		{
			return i;
		}
	}

	return {};
}

bool BuddySystem::GetBlockState( uint32 level, size_t blockIndex ) const noexcept
{
	return m_freeFlags[GetBlockStateOffset( level ) + blockIndex];
}

void BuddySystem::SetBlockState( uint32 level, size_t blockIndex, bool isFree ) noexcept
{
	m_freeFlags[GetBlockStateOffset( level ) + blockIndex] = isFree;
}

size_t BuddySystem::SplitBlock( uint32 srcLevel, uint32 destLevel, size_t srcIndex ) noexcept
{
	for ( uint32 i = srcLevel; i > destLevel; --i )
	{
		SetBlockState( i, srcIndex, false );

		srcIndex *= 2;

		SetBlockState( i - 1, srcIndex, true );
		SetBlockState( i - 1, srcIndex + 1, true );
	}

	return srcIndex;
}

void BuddySystem::MergeBlcok( size_t blockIndex, uint32 level ) noexcept
{
	size_t parentIndex = blockIndex / 2;

	for ( uint32 l = level + 1; l < m_numLevel; ++l )
	{
		uint32 childLevel = l - 1;
		size_t leftIndex = parentIndex * 2;
		size_t rightIndex = leftIndex + 1;

		if ( ( GetBlockState( childLevel, leftIndex ) == false )
			|| ( GetBlockState( childLevel, rightIndex ) == false ) )
		{
			break;
		}

		SetBlockState( childLevel, leftIndex, false );
		SetBlockState( childLevel, rightIndex, false );
		SetBlockState( l, parentIndex, true );

		parentIndex /= 2;
	}
}
