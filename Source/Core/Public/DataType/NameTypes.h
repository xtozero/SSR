#pragma once

#include "GuideTypes.h"
#include "SizedTypes.h"

#include <compare>
#include <string_view>

static constexpr uint32 NameSize = 1024;

struct NameEntryId
{
public:
	uint32 Value() const
	{
		return m_value;
	}

	explicit NameEntryId( uint32 value ) : m_value( value ) {}
	NameEntryId() = default;

	std::strong_ordering operator<=>( const NameEntryId& other ) const = default;

private:
	uint32 m_value = 0;
};

class NameEntryHandle
{
public:
	uint32 Block() const
	{
		return m_block;
	}

	uint32 Offset() const
	{
		return m_offset;
	}

	NameEntryId ToNameEntryId() const;

	NameEntryHandle( uint32 block, uint32 offset );
	NameEntryHandle( const NameEntryId id );

private:
	uint32 m_block = 0;
	uint32 m_offset = 0;
};

struct NameEntry
{
	uint16 m_len = 0;
	char m_str[NameSize];
};

class INamePool
{
public:
	virtual NameEntryId Store( const std::string_view& view ) = 0;
	virtual NameEntryId Find( const std::string_view& view ) = 0;
	virtual NameEntry& Resolve( const NameEntryHandle& handle ) const = 0;

	INamePool() = default;
	virtual ~INamePool() = default;
	INamePool( const INamePool& ) = delete;
	INamePool& operator=( const INamePool& ) = delete;
	INamePool( INamePool&& ) = delete;
	INamePool& operator=( INamePool&& ) = delete;
};

class Name
{
public:
	explicit Name( NameEntryId id ) : m_id( id ) {}
	explicit Name( const char* str ) : Name( Make( str ) ) {}
	explicit Name( const std::string& str ) : Name( Make( str ) ) {}
	Name() = default;

	std::string_view Str() const;

	std::strong_ordering operator<=>( const Name& other ) const = default;

private:
	static Name Make( const std::string_view& view );

	NameEntryId m_id;
};

Owner<INamePool*> CreateNamePool();
void DestroyNamePool( Owner<INamePool*> pNamePool );