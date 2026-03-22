#pragma once

#include "SizedTypes.h"

#include <type_traits>

template <typename T, typename Ret>
concept HasGetHash = requires( std::remove_pointer_t<T> t )
{
	{ t.GetHash() } -> std::same_as<Ret>;
};

template <typename T> requires (!HasGetHash<T, uint16> || !std::is_pointer_v<T>)
void HashCombine( uint16& seed, const T& val )
{
	seed ^= std::hash<T>{}(val)+0x9e37U + ( seed << 3 ) + ( seed >> 1 );
}

template <typename T> requires (!HasGetHash<T, uint32> || !std::is_pointer_v<T>)
void HashCombine( uint32& seed, const T& val )
{
	seed ^= std::hash<T>{}(val)+0x9e3779b9U + ( seed << 6 ) + ( seed >> 2 );
}

template <typename T> requires (!HasGetHash<T, uint64> || !std::is_pointer_v<T>)
void HashCombine( uint64& seed, const T& val )
{
	seed ^= std::hash<T>{}(val)+0x9e3779b97f4a7c15LLU + ( seed << 12 ) + ( seed >> 4 );
}

template <typename T> requires HasGetHash<T, uint16>
void HashCombine( uint16& seed, const T* ptr )
{
	HashCombine( seed, ptr ? ptr->GetHash() : 0 );
}

template <typename T> requires HasGetHash<T, uint32>
void HashCombine( uint32& seed, const T* ptr )
{
	HashCombine( seed, ptr ? ptr->GetHash() : 0 );
}

template <typename T> requires HasGetHash<T, uint64>
void HashCombine( uint64& seed, const T* ptr )
{
	HashCombine( seed, ptr ? ptr->GetHash() : 0 );
}