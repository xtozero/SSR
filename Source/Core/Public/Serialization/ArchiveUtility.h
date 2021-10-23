#pragma once

#include "Math/CXMFloat.h"

#include <map>
#include <vector>

class Archive;

inline Archive& operator<<( Archive& ar, CXMFLOAT4& v )
{
	ar << v[0] << v[1] << v[2] << v[3];
	return ar;
}

inline Archive& operator<<( Archive& ar, CXMFLOAT3& v )
{
	ar << v[0] << v[1] << v[2];
	return ar;
}

inline Archive& operator<<( Archive& ar, CXMFLOAT2& v )
{
	ar << v[0] << v[1];
	return ar;
}

template <typename Type, typename Alloc>
Archive& operator<<( Archive& ar, std::vector<Type, Alloc>& v )
{
	if ( ar.IsWriteMode( ) )
	{
		ar << static_cast<uint32>( v.size( ) );
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
	if ( ar.IsWriteMode( ) )
	{
		ar << static_cast<uint32>( v.size( ) );
		for ( auto& elem : v )
		{
			ar << elem.first << elem.second;
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
