#pragma once

#include "Archive.h"
#include "ArchiveUtility.h"
#include "TypeInfoMacros.h"

#include <cassert>

using SerializeFunc = void (*)( void* object, Archive& ar );

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
			( static_cast<TClass*>( object )->*m_ptr )[index] = value;
		}
		else
		{
			static_cast<TClass*>( object )->*m_ptr = value;
		}
	}

	explicit PropertyHandler( MemberPtr ptr ) :
		m_ptr( ptr ) {}

private:
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

		operator T& ( )
		{
			return *m_value;
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

	void Serialize( void* object, Archive& ar ) const
	{
		m_serializer( object, ar );
	}

	Property( TypeInfo& owner, const PropertyInitializer& initializer ) :
		m_name( initializer.m_name ),
		m_type( initializer.m_type ),
		m_handler( initializer.m_handler ),
		m_serializer( initializer.m_serializer )
	{
		owner.AddProperty( this );
	}

private:
	const char* m_name = nullptr;
	const TypeInfo& m_type;
	const PropertyHandlerBase& m_handler;
	const SerializeFunc m_serializer = nullptr;
};

template <typename T, size_t N>
constexpr size_t SizeOfArray( const T( & )[N] )
{
	return N * SizeOfArray( T{} );
}

template <typename T>
constexpr size_t SizeOfArray( const T& )
{
	return 1;
}

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

						for ( size_t i = 0; i < SizeOfArray( T{} ); ++i )
						{
							ar << *elem++;
						}
					}
					else
					{
						ar << static_cast<TClass*>( object )->*ptr;
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

						for ( size_t i = 0; i < SizeOfArray( T{} ); ++i )
						{
							ar << *elem++;
						}
					}
					else
					{
						ar << *ptr;
					}
				}
			};
			static Property property( typeInfo, intializer );
		}
	}
};