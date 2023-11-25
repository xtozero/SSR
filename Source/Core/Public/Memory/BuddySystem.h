#pragma once

#include "SizedTypes.h"

#include <optional>
#include <vector>

class BuddySystem final
{
public:
	bool Initialize( size_t size, size_t minBlockSize ) noexcept;

	[[nodiscard]] std::optional<size_t> Allocate( size_t size );
	void Deallocate( size_t offset );

	size_t Capacity() const noexcept;
	size_t UsedSize() const noexcept;
	size_t AvailableSize() const noexcept;

#ifdef DoUnitTest
	size_t MinimumAllocSize() const
	{
		return m_minBlockSize;
	}
#endif

private:
	size_t GetSuitableBlockSize( size_t size ) const noexcept;
	uint32 GetLevel( size_t size ) const noexcept;

	size_t GetBlockStateOffset( uint32 level ) const noexcept;
	std::optional<size_t> GetFreeBlock( uint32 level ) const noexcept;
	bool GetBlockState( uint32 level, size_t blockIndex ) const noexcept;
	void SetBlockState( uint32 level, size_t blockIndex, bool isFree ) noexcept;
	size_t SplitBlock( uint32 srcLevel, uint32 destLevel, size_t srcIndex ) noexcept;
	void MergeBlcok( size_t blockIndex, uint32 level ) noexcept;

	size_t m_size = 0;
	size_t m_minBlockSize = 0;

	size_t m_usedSize = 0;

	uint32 m_numLevel = 0;

	std::vector<bool> m_freeFlags;
	std::vector<int32> m_usedBlockLevels;
};