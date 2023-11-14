#pragma once

#include "Archive.h"
#include "ArchiveUtility.h"
#include "PropertyParser.h"
#include "TypeInfoMacros.h"

#include <cassert>
#include <string>

using SerializeFunc = void (*)( void* object, Archive& ar );
using ParseFunc = void ( * )( void* object, const std::string& s );

class PropertyHandlerBase
{
	GENERATE_CLASS_TYPE_INFO( PropertyHandlerBase )

public:
	virtual ~PropertyHandlerBase() = default;
};

template <typename T>
class IPropertyHandler : public PropertyHandlerBase
{
	GENERATE_CLASS_TYPE_INFO( IPropertyHandler )
	using ElementType = std::remove_all_extents_t<T>;

public:
	virtual ElementType& Get( void* object, size_t index = 0 ) const = 0;
	virtual void Set( void* object, const ElementType& value, size_t index = 0 ) const = 0;
};

template <typename TClass, typename T>
class PropertyHandler : public IPropertyHandler<T>
{
	GENERATE_CLASS_TYPE_INFO( PropertyHandler )
	using MemberPtr = T TClass::*;
	using ElementType = std::remove_all_extents_t<T>;

public:
	virtual ElementType& Get( void* object, size_t index = 0 ) const override
	{
		if constexpr ( std::is_array_v<T> )
		{
			return ( static_cast<TClass*>( object )->*m_ptr )[index];
		}
		else
		{
			return static_cast<TClass*>( object )->*m_ptr;
		}
	}

	virtual void Set( void* object, const ElementType& value, size_t index = 0 ) const override
	{
		if constexpr ( std::is_array_v<T> )
		{
			Set( ( static_cast<TClass*>( object )->*m_ptr )[index], value );
		}
		else
		{
			Set( static_cast<TClass*>( object )->*m_ptr, value );
		}
	}

	explicit PropertyHandler( MemberPtr ptr ) :
		m_ptr( ptr ) {}

private:
	template <typename T>
	void Set( T& dest, const T& src ) const
	{
		dest = src;
	}

	template <typename KeyType, typename ValueType, typename Pred, typename Alloc>
	void Set( std::map<KeyType, ValueType, Pred, Alloc>& dest, const std::map<KeyType, ValueType, Pred, Alloc>& src ) const
	{
		if constexpr ( std::is_copy_assignable_v<KeyType> && std::is_copy_assignable_v<ValueType> )
		{
			dest = src;
		}
	}

	MemberPtr m_ptr = nullptr;
};

template <typename TClass, typename T>
class StaticPropertyHandler : public IPropertyHandler<T>
{
	GENERATE_CLASS_TYPE_INFO( StaticPropertyHandler )
	using ElementType = std::remove_all_extents_t<T>;

public:
	virtual ElementType& Get( [[maybe_unused]] void* object, size_t index = 0 ) const override
	{
		if constexpr ( std::is_array_v<T> )
		{
			return ( *m_ptr )[index];
		}
		else
		{
			return *m_ptr;
		}
	}

	virtual void Set( [[maybe_unused]] void* object, const ElementType& value, size_t index = 0 ) const override
	{
		if constexpr ( std::is_array_v<T> )
		{
			( *m_ptr )[index] = value;
		}
		else
		{
			*m_ptr = value;
		}
	}

	explicit StaticPropertyHandler( T* ptr ) :
		m_ptr( ptr ) {}

private:
	T* m_ptr = nullptr;
};

struct PropertyInitializer
{
	const char* m_name = nullptr;
	const TypeInfo& m_type;
	const PropertyHandlerBase& m_handler;
	const SerializeFunc m_serializer = nullptr;
	const ParseFunc m_parser = nullptr;
};

class Property
{
public:
	const char* GetName() const
	{
		return m_name;
	}

	template <typename T>
	struct ReturnValueWrapper
	{
	public:
		explicit ReturnValueWrapper( T& value ) :
			m_value( &value ) {}
		ReturnValueWrapper() = default;

		ReturnValueWrapper& operator=( const T& value )
		{
			*m_value = value;
			return *this;
		}

		operator T& ()
		{
			return *m_value;
		}

		T& Get()
		{
			return *m_value;
		}

		const T& Get() const
		{
			return *m_value;
		}

		friend bool operator==( const ReturnValueWrapper& lhs, const ReturnValueWrapper& rhs )
		{
			return *lhs.m_value == *rhs.m_value;
		}

		friend bool operator!=( const ReturnValueWrapper& lhs, const ReturnValueWrapper& rhs )
		{
			return !( lhs == rhs );
		}

	private:
		T* m_value = nullptr;
	};

	template <typename T>
	ReturnValueWrapper<T> Get( void* object ) const
	{
		if ( m_handler.GetTypeInfo().IsChildOf<IPropertyHandler<T>>() )
		{
			auto concreteHandler = static_cast<const IPropertyHandler<T>*>( &m_handler );
			return ReturnValueWrapper( concreteHandler->Get( object ) );
		}
		else
		{
			assert( false && "Property::Get<T> - Invalied casting" );
			return {};
		}
	}

	template <typename T>
	void Set( void* object, const T& value ) const
	{
		if ( m_handler.GetTypeInfo().IsChildOf<IPropertyHandler<T>>() )
		{
			auto concreteHandler = static_cast<const IPropertyHandler<T>*>( &m_handler );
			concreteHandler->Set( object, value );
		}
		else
		{
			assert( false && "Property::Set<T> - Invalied casting" );
		}
	}

	template <typename TClass, typename T>
	ReturnValueWrapper<T> Get( void* object ) const
	{
		const TypeInfo& typeinfo = m_handler.GetTypeInfo();
		if ( typeinfo.IsA<PropertyHandler<TClass, T>>() )
		{
			auto concreteHandler = static_cast<const PropertyHandler<TClass, T>&>( m_handler );
			return ReturnValueWrapper( concreteHandler.Get( object ) );
		}
		else if ( typeinfo.IsA<StaticPropertyHandler<TClass, T>>() )
		{
			auto concreteHandler = static_cast<const StaticPropertyHandler<TClass, T>&>( m_handler );
			return ReturnValueWrapper( concreteHandler.Get( object ) );
		}
		else
		{
			assert( false && "Property::Get<TClass, T> - Invalied casting" );
			return {};
		}
	}

	template <typename TClass, typename T>
	void Set( void* object, const T& value ) const
	{
		const TypeInfo& typeinfo = m_handler.GetTypeInfo();
		if ( typeinfo.IsA<PropertyHandler<TClass, T>>() )
		{
			auto concreteHandler = static_cast<const PropertyHandler<TClass, T>&>( m_handler );
			concreteHandler.Set( object, value );
		}
		else if ( typeinfo.IsA<StaticPropertyHandler<TClass, T>>() )
		{
			auto concreteHandler = static_cast<const StaticPropertyHandler<TClass, T>&>( m_handler );
			concreteHandler.Set( object, value );
		}
		else
		{
			assert( false && "Property::Set<TClass, T> - Invalied casting" );
		}
	}

	void Parse( void* object, const std::string& s ) const
	{
		if ( m_parser )
		{
			m_parser( object, s );
		}
	}

	void Serialize( void* object, Archive& ar ) const
	{
		m_serializer( object, ar );
	}

	const TypeInfo& GetTypeInfo() const
	{
		return m_type;
	}

	Property( TypeInfo& owner, const PropertyInitializer& initializer )
		: m_name( initializer.m_name )
		, m_type( initializer.m_type )
		, m_handler( initializer.m_handler )
		, m_serializer( initializer.m_serializer )
		, m_parser( initializer.m_parser )
	{
		owner.AddProperty( this );
	}

private:
	const char* m_name = nullptr;
	const TypeInfo& m_type;
	const PropertyHandlerBase& m_handler;
	const SerializeFunc m_serializer = nullptr;
	const ParseFunc m_parser = nullptr;
};

template <typename T>
struct SizeOfArray
{
	constexpr static uint32 value = 1;
};

template <typename T, size_t N>
struct SizeOfArray<T[N]>
{
	constexpr static uint32 value = SizeOfArray<T>::value * N;
};

template <typename TClass, typename T, typename TPtr, TPtr ptr>
class PropertyRegister
{
public:
	PropertyRegister( const char* name, TypeInfo& typeInfo )
	{
		if constexpr ( std::is_member_pointer_v<TPtr> )
		{
			static PropertyHandler<TClass, T> handler( ptr );
			static PropertyInitializer intializer = {
			.m_name = name,
			.m_type = TypeInfo::GetStaticTypeInfo<T>(),
			.m_handler = handler,
			.m_serializer = +[]( void* object, Archive& ar )
				{
					if constexpr ( std::is_array_v<T> )
					{
						using ElementType = std::remove_all_extents_t<T>;
						auto elem = reinterpret_cast<ElementType*>( &( static_cast<TClass*>( object )->*ptr ) );

						for ( size_t i = 0; i < SizeOfArray<T>::value; ++i )
						{
							ar << *elem++;
						}
					}
					else
					{
						ar << static_cast<TClass*>( object )->*ptr;
					}
				},
			.m_parser = +[]( void* object, const std::string& s )
				{
					if constexpr ( std::is_array_v<T> )
					{
						using ElementType = std::remove_all_extents_t<T>;
						auto elem = reinterpret_cast<ElementType*>( &( static_cast<TClass*>( object )->*ptr ) );

						ParseArray( elem, SizeOfArray<T>::value, s );
					}
					else
					{
						Parse( static_cast<TClass*>( object )->*ptr, s );
					}
				}
			};
			static Property property( typeInfo, intializer );
		}
		else
		{
			static StaticPropertyHandler<TClass, T> handler( ptr );
			static PropertyInitializer intializer = {
			.m_name = name,
			.m_type = TypeInfo::GetStaticTypeInfo<T>(),
			.m_handler = handler,
			.m_serializer = +[]( void* object, Archive& ar )
				{
					if constexpr ( std::is_array_v<T> )
					{
						using ElementType = std::remove_all_extents_t<T>;
						auto elem = reinterpret_cast<ElementType*>( &*ptr );

						for ( size_t i = 0; i < SizeOfArray<T>::value; ++i )
						{
							ar << *elem++;
						}
					}
					else
					{
						ar << *ptr;
					}
				},
			.m_parser = +[]( void* object, const std::string& s )
				{
					if constexpr ( std::is_array_v<T> )
					{
						using ElementType = std::remove_all_extents_t<T>;
						auto elem = reinterpret_cast<ElementType*>( &*ptr );

						ParseArray( elem, SizeOfArray<T>::value, s );
					}
					else
					{
						Parse( *ptr, s );
					}
				}
			};
			static Property property( typeInfo, intializer );
		}
	}
};