#pragma once

#include <type_traits>

#define ENUM_CLASS_FLAGS( Enum )	\
inline Enum& operator|=( Enum& lhs, Enum rhs )	\
{	\
	using UnderlyType = std::underlying_type_t<Enum>;	\
	lhs = static_cast<Enum>( static_cast<UnderlyType>( lhs ) | static_cast<UnderlyType>( rhs ) );	\
	return lhs;	\
}	\
\
inline Enum& operator&=( Enum& lhs, Enum rhs )	\
{	\
	using UnderlyType = std::underlying_type_t<Enum>;	\
	lhs = static_cast<Enum>( static_cast<UnderlyType>( lhs ) & static_cast<UnderlyType>( rhs ) );	\
	return lhs;	\
}	\
\
inline Enum& operator^=( Enum& lhs, Enum rhs )	\
{	\
	using UnderlyType = std::underlying_type_t<Enum>;	\
	lhs = static_cast<Enum>( static_cast<UnderlyType>( lhs ) ^ static_cast<UnderlyType>( rhs ) );	\
	return lhs;	\
}	\
\
inline constexpr Enum operator|( Enum lhs, Enum rhs )	\
{	\
	using UnderlyType = std::underlying_type_t<Enum>;	\
	return static_cast<Enum>( static_cast<UnderlyType>( lhs ) | static_cast<UnderlyType>( rhs ) );	\
}	\
\
inline constexpr Enum operator&( Enum lhs, Enum rhs )	\
{	\
	using UnderlyType = std::underlying_type_t<Enum>;	\
	return static_cast<Enum>( static_cast<UnderlyType>( lhs ) & static_cast<UnderlyType>( rhs ) );	\
}	\
\
inline constexpr Enum operator^( Enum lhs, Enum rhs )	\
{	\
	using UnderlyType = std::underlying_type_t<Enum>;	\
	return static_cast<Enum>( static_cast<UnderlyType>( lhs ) ^ static_cast<UnderlyType>( rhs ) );	\
}	\
\
inline constexpr bool operator!( Enum e )	\
{	\
	using UnderlyType = std::underlying_type_t<Enum>;	\
	return !static_cast<UnderlyType>( e );	\
}	\
\
inline constexpr Enum operator~( Enum e )	\
{	\
	using UnderlyType = std::underlying_type_t<Enum>;	\
	return static_cast<Enum>( ~static_cast<UnderlyType>( e ) );	\
}	\
\

template <typename Enum>
inline bool HasAllFlags( Enum flags, Enum value )
{
	using UnderlyType = std::underlying_type_t<Enum>;
	return ( static_cast<UnderlyType>( flags ) & static_cast<UnderlyType>( value ) ) == static_cast<UnderlyType>( value );
}

template <typename Enum>
inline bool HasAnyFlags( Enum flags, Enum value )
{
	using UnderlyType = std::underlying_type_t<Enum>;
	return ( static_cast<UnderlyType>( flags ) & static_cast<UnderlyType>( value ) ) != 0;
}
