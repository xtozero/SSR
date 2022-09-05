#pragma once

#include "NameTypes.h"
#include "Math/Vector.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"

#include <chrono>
#include <filesystem>
#include <map>
#include <set>
#include <vector>
#include <utility>

class Archive;

inline Archive& operator<<( Archive& ar, Vector& v )
{
	ar << v[0] << v[1] << v[2];

	return ar;
}

inline Archive& operator<<( Archive& ar, Vector2& v )
{
	ar << v[0] << v[1];

	return ar;
}

inline Archive& operator<<( Archive& ar, Vector4& v )
{
	ar << v[0] << v[1] << v[2] << v[3];

	return ar;
}

inline Archive& operator<<( Archive& ar, Name& name )
{
	if ( ar.IsWriteMode() )
	{
		ar << name.Str();
	}
	else
	{
		char buf[NameSize] = {};
		ar << buf;
		std::construct_at( &name, buf );
	}

	return ar;
}

inline Archive& operator<<( Archive& ar, std::filesystem::file_time_type& lastWriteTime )
{
	if ( ar.IsWriteMode() )
	{
		uint64 count = lastWriteTime.time_since_epoch().count();
		ar << count;
	}
	else
	{
		uint64 count = 0;
		ar << count;
		std::filesystem::file_time_type::duration duration( count );
		std::construct_at( &lastWriteTime, duration );
	}

	return ar;
}

template <typename Type, typename Alloc>
Archive& operator<<( Archive& ar, std::vector<Type, Alloc>& v )
{
	if ( ar.IsWriteMode() )
	{
		ar << static_cast<uint32>( v.size() );
	}
	else
	{
		uint32 size = 0;
		ar << size;
		v.resize( size );
	}

	for ( auto& elem : v )
	{
		ar << elem;
	}

	return ar;
}

template <typename KeyType, typename ValueType, typename Pred, typename Alloc>
Archive& operator<<( Archive& ar, std::map<KeyType, ValueType, Pred, Alloc>& v )
{
	if ( ar.IsWriteMode() )
	{
		ar << static_cast<uint32>( v.size() );
		for ( auto& elem : v )
		{
			KeyType key = elem.first;
			ar << key << elem.second;
		}
	}
	else
	{
		uint32 size = 0;
		ar << size;
		for ( uint32 i = 0; i < size; ++i )
		{
			KeyType key;
			ar << key;
			ar << v[key];
		}
	}

	return ar;
}

template <typename First, typename Second>
Archive& operator<<( Archive& ar, std::pair<First, Second>& v )
{
	ar << v.first << v.second;
	return ar;
}

template <typename KeyType, typename Pred, typename Alloc>
Archive& operator<<( Archive& ar, std::set<KeyType, Pred, Alloc>& v )
{
	if ( ar.IsWriteMode() )
	{
		ar << static_cast<uint32>( v.size() );
		for ( auto& elem : v )
		{
			ar << elem;
		}
	}
	else
	{
		uint32 size = 0;
		ar << size;
		for ( uint32 i = 0; i < size; ++i )
		{
			KeyType elem;
			ar << elem;
			v.emplace( elem );
		}
	}

	return ar;
}