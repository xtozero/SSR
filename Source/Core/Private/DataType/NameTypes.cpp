#include "NameTypes.h"

#include "CrcHash.h"
#include "LibraryTool/InterfaceFactories.h"

#include <cassert>
#include <mutex>
#include <shared_mutex>

namespace
{
	constexpr uint32 NameMaxBlockBits = 13;
	constexpr uint32 NameBlockOffsetBits = 16;
	constexpr uint32 NameMaxBlocks = 1 << NameMaxBlockBits;
	constexpr uint32 NameBlockOffsets = 1 << NameBlockOffsetBits;

	constexpr uint32 NamePoolShardBits = 10;
	constexpr uint32 NamePoolShards = 1 << NamePoolShardBits;
	constexpr uint32 NamePoolShardMask = NamePoolShards - 1;
	constexpr uint32 NamePoolInitialSlotBits = 8;
	constexpr uint32 NamePoolInitialSlotsPerShard = 1 << NamePoolInitialSlotBits;

	INamePool& GetNamePool()
	{
		return *GetInterface<INamePool>();
	}
}

NameEntryId NameEntryHandle::ToNameEntryId() const
{
	return NameEntryId( m_block << NameBlockOffsetBits | m_offset );
}

NameEntryHandle::NameEntryHandle( uint32 block, uint32 offset )
	: m_block( block )
	, m_offset( offset ) {}

NameEntryHandle::NameEntryHandle( const NameEntryId id )
	: m_block( id.Value() >> NameBlockOffsetBits )
	, m_offset( id.Value() & ( NameBlockOffsets - 1 ) ) {}

class NameEntryAllocator
{
public:
	NameEntryHandle Allocate( uint32 bytes )
	{
		bytes = static_cast<uint32>( ( (uptrint)bytes + Stride - 1 ) & ~( Stride - 1 ) );

		std::lock_guard lk( m_mutex );

		if ( BlockSizeBytes - m_currentByte < bytes )
		{
			AllocateNewBlock();
		}

		uint32 offset = m_currentByte;
		m_currentByte += bytes;

		return NameEntryHandle( m_currentBlock, offset / Stride );
	}

	NameEntryHandle Create( const std::string_view& view )
	{
		auto length = static_cast<uint32>( offsetof( NameEntry, m_str ) + view.length() + 1 );
		NameEntryHandle handle = Allocate( length );
		NameEntry& entry = Resolve( handle );

		assert( view.length() < NameSize );
		entry.m_len = static_cast<uint16>( view.length() );

		std::strncpy( entry.m_str, view.data(), view.length() );
		entry.m_str[view.length()] = '\0';

		return handle;
	}

	NameEntry& Resolve( NameEntryHandle handle ) const
	{
		return *reinterpret_cast<NameEntry*>( m_blocks[handle.Block()] + Stride * handle.Offset() );
	}

	NameEntryAllocator()
	{
		m_blocks[0] = new uint8[BlockSizeBytes];
	}

	~NameEntryAllocator()
	{
		for ( uint32 i = 0; i <= m_currentBlock; ++i )
		{
			delete[] m_blocks[i];
		}
	}

	NameEntryAllocator( const NameEntryAllocator& ) = delete;
	NameEntryAllocator& operator=( const NameEntryAllocator& ) = delete;
	NameEntryAllocator( NameEntryAllocator&& ) = delete;
	NameEntryAllocator& operator=( NameEntryAllocator&& ) = delete;

private:
	void AllocateNewBlock()
	{
		++m_currentBlock;
		m_currentByte = 0;

		m_blocks[m_currentBlock] = new uint8[BlockSizeBytes];
	}

	static constexpr uint32 Stride = alignof( NameEntry );
	static constexpr uint32 BlockSizeBytes = Stride * NameBlockOffsets;

	std::mutex m_mutex;
	uint32 m_currentBlock = 0;
	uint32 m_currentByte = 0;
	uint8* m_blocks[NameMaxBlocks];
};

struct NameSlot
{
	static constexpr uint32 EntryIdBits = NameMaxBlockBits + NameBlockOffsetBits;
	static constexpr uint32 EntryIdMask = ( 1 << EntryIdBits ) - 1;
	static constexpr uint32 ProbeHashShift = EntryIdBits;
	static constexpr uint32 ProbeHashMask = ~EntryIdMask;

	NameSlot( const NameEntryId& id, uint32 probeHash ) : m_hashAndId( probeHash | id.Value() ) {}
	NameSlot() = default;

	NameEntryId Id() const
	{
		return NameEntryId( EntryIdMask & m_hashAndId );
	}

	uint32 ProbeHash() const
	{
		return ProbeHashMask & m_hashAndId;
	}

	bool Used() const
	{
		return !!m_hashAndId;
	}

	friend bool operator==( NameSlot lhs, NameSlot rhs )
	{
		return lhs.m_hashAndId == rhs.m_hashAndId;
	}

private:
	uint32 m_hashAndId = 0;
};

class NameHash
{
public:
	NameHash( const char* str, size_t len )
	{
		uint64 hash = Crc64Hash( str, static_cast<uint32>( len ) );
		auto hi = static_cast<uint32>( hash >> 32 );
		auto lo = static_cast<uint32>( hash );

		m_shardIndex = hi & NamePoolShardMask;
		m_unmaskedSlotIndex = lo;
		m_slotProbeHash = hi & NameSlot::ProbeHashMask;
	}

	NameHash( const std::string_view& view ) : NameHash( view.data(), view.length() )
	{}

	uint32 ShardIndex() const
	{
		return m_shardIndex;
	}

	uint32 GetProbeStart( uint32 slotMask ) const
	{
		return m_unmaskedSlotIndex & slotMask;
	}

	uint32 SlotProbeHash() const
	{
		return m_slotProbeHash;
	}

private:
	uint32 m_shardIndex = 0;
	uint32 m_unmaskedSlotIndex = 0;
	uint32 m_slotProbeHash = 0;
};

NameHash HashLowerCase( const char* str, size_t len )
{
	char lowerStr[NameSize] = {};
	for ( size_t i = 0; i < len; ++i )
	{
		lowerStr[i] = static_cast<char>( std::tolower( str[i] ) );
	}

	return NameHash( lowerStr, len );
}

NameHash HashName( const std::string_view& view )
{
	return HashLowerCase( view.data(), view.length() );
}

struct NameValue
{
	NameValue( const std::string_view& view )
		: m_view( view )
		, m_hash( HashName( view ) ) {}

	std::string_view m_view;
	NameHash m_hash;
};

class alignas(64) NamePoolShardBase
{
public:
	void Initialize( NameEntryAllocator& allocator )
	{
		m_entries = &allocator;
		m_slots = new NameSlot[NamePoolInitialSlotsPerShard];
		m_capacityMask = NamePoolInitialSlotsPerShard - 1;
	}

	uint32 Capacity() const
	{
		return m_capacityMask + 1;
	}

	NamePoolShardBase() = default;

	~NamePoolShardBase()
	{
		delete[] m_slots;
		m_slots = nullptr;
		m_capacityMask = 0;
	}

	NamePoolShardBase( const NamePoolShardBase& ) = delete;
	NamePoolShardBase& operator=( const NamePoolShardBase& ) = delete;
	NamePoolShardBase( NamePoolShardBase&& ) = delete;
	NamePoolShardBase& operator=( NamePoolShardBase&& ) = delete;

protected:
	static bool EntryEqualsValue( const NameEntry& entry, const NameValue& value )
	{
		return ( entry.m_len == value.m_view.length() ) &&
#ifdef _WIN32
		( _strnicmp( entry.m_str, value.m_view.data(), value.m_view.length() ) == 0 );
#else
		( strnicmp( entry.m_str, value.m_view.data(), value.m_view.length() ) == 0 );
#endif
	}

	static constexpr uint32 LoadFactorQuotient = 9;
	static constexpr uint32 LoadFactorDivisor = 10;

	mutable std::shared_mutex m_mutex;
	uint32 m_usedSlots = 0;
	uint32 m_capacityMask = 0;
	NameSlot* m_slots = nullptr;
	NameEntryAllocator* m_entries = nullptr;
};

class NamePoolShard : public NamePoolShardBase
{
public:
	NameEntryId Find( const NameValue& value ) const
	{
		std::shared_lock lk( m_mutex );

		return Probe( value ).Id();
	}

	NameEntryId Insert( const NameValue& value )
	{
		std::unique_lock lk( m_mutex );

		NameSlot& slot = Probe( value );

		if ( slot.Used() )
		{
			return slot.Id();
		}

		NameEntryId newEntryId = m_entries->Create( value.m_view ).ToNameEntryId();

		ClaimSlot( slot, NameSlot( newEntryId, value.m_hash.SlotProbeHash() ) );

		return newEntryId;
	}

private:
	void ClaimSlot( NameSlot& unusedSlot, NameSlot newSlot )
	{
		unusedSlot = newSlot;

		++m_usedSlots;
		if ( m_usedSlots * LoadFactorDivisor >= LoadFactorQuotient * Capacity() )
		{
			Grow();
		}
	}

	void Grow()
	{
		NameSlot* oldSlots = m_slots;
		[[maybe_unused]] uint32 oldUsedSlots = m_usedSlots;
		uint32 oldCapacity = Capacity();
		uint32 newCapacity = oldCapacity << 1;

		m_slots = new NameSlot[newCapacity];
		m_usedSlots = 0;
		m_capacityMask = newCapacity - 1;

		for ( uint32 i = 0; i < oldCapacity; ++i )
		{
			const NameSlot& oldSlot = oldSlots[i];
			if ( oldSlot.Used() )
			{
				NameHash hash = Rehash( oldSlot.Id() );
				NameSlot& newSlot = Probe( hash, []( [[maybe_unused]] NameSlot slot ) { return false; } );
				newSlot = oldSlot;
				++m_usedSlots;
			}
		}

		assert( oldUsedSlots == m_usedSlots );

		delete[] oldSlots;
	}

	NameSlot& Probe( const NameValue& value ) const
	{
		auto pred = [&]( NameSlot slot )
		{
			if ( slot.ProbeHash() == value.m_hash.SlotProbeHash() )
			{
				return EntryEqualsValue( m_entries->Resolve( slot.Id() ), value );
			}

			return false;
		};
		return Probe( value.m_hash, pred );
	}

	template <typename Pred>
	NameSlot& Probe( const NameHash& hash, Pred pred ) const
	{
		for ( uint32 i = hash.GetProbeStart( m_capacityMask ); true; i = ( i + 1 ) & m_capacityMask )
		{
			NameSlot& slot = m_slots[i];
			if ( ( slot.Used() == false ) || pred( slot ) )
			{
				return slot;
			}
		}
	}

	NameHash Rehash( const NameEntryId& id )
	{
		const NameEntry& entry = m_entries->Resolve( id );
		return HashName( entry.m_str );
	}
};

class NamePool : public INamePool
{
public:
	virtual NameEntryId Store( const std::string_view& view ) override
	{
		NameValue value( view );
		NameEntryId entryId = m_shards[value.m_hash.ShardIndex()].Insert( value );

		return entryId;
	}

	virtual NameEntryId Find( const std::string_view& view ) override
	{
		NameValue value( view );
		return m_shards[value.m_hash.ShardIndex()].Find( value );
	}

	virtual NameEntry& Resolve( const NameEntryHandle& handle ) const override
	{
		return m_entries.Resolve( handle );
	}

	NamePool()
	{
		for ( NamePoolShardBase& shard : m_shards )
		{
			shard.Initialize( m_entries );
		}
	}

private:
	NameEntryAllocator m_entries;
	NamePoolShard m_shards[NamePoolShards];
};

std::string_view Name::Str() const
{
	INamePool& pool = GetNamePool();
	const NameEntry& entry = pool.Resolve( m_id );
	return { entry.m_str, entry.m_len };
}

const char* Name::CStr() const
{
	return Str().data();
}

Name Name::Make( const std::string_view& view )
{
	INamePool& pool = GetNamePool();
	NameEntryId id = pool.Store( view );

	Name name( id );
#ifdef _DEBUG
	name.m_str = name.Str().data();

#ifdef _WIN32
	bool cmp = ( _strnicmp( name.Str().data(), view.data(), view.length()) == 0);
#else
	bool cmp = ( strnicmp( ( name.Str().data(), view.data(), view.length() ) == 0 );
#endif

	assert( cmp );
#endif

	return name;
}

Owner<INamePool*> CreateNamePool()
{
	return new NamePool();
}

void DestroyNamePool( Owner<INamePool*> pNamePool )
{
	delete pNamePool;
}