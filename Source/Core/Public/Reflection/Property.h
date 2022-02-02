#pragma once

#include "TypeInfoMacros.h"

#include <cassert>

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

public:
	virtual T& Get( void* object ) const = 0;
	virtual void Set( void* object, const T& value ) const = 0;
};

template <typename TClass, typename T>
class PropertyHandler : public IPropertyHandler<T>
{
	GENERATE_CLASS_TYPE_INFO( PropertyHandler )
	using MemberPtr = T TClass::*;

public:
	virtual T& Get( void* object ) const override
	{
		return static_cast<TClass*>( object )->*m_ptr;
	}

	virtual void Set( void* object, const T& value ) const override
	{
		static_cast<TClass*>( object )->*m_ptr = value;
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

public:
	virtual T& Get( [[maybe_unused]] void* object ) const override
	{
		return *m_ptr;
	}

	virtual void Set( [[maybe_unused]] void* object, const T& value ) const override
	{
		*m_ptr = value;
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
};

class Property
{
public:
	const char* Name() const
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

	Property( TypeInfo& owner, const PropertyInitializer& initializer ) :
		m_name( initializer.m_name ),
		m_type( initializer.m_type ),
		m_handler( initializer.m_handler )
	{
		owner.AddProperty( this );
	}

private:
	const char* m_name = nullptr;
	const TypeInfo& m_type;
	const PropertyHandlerBase& m_handler;
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
			.m_handler = handler
			};
			static Property property( typeInfo, intializer );
		}
		else
		{
			static StaticPropertyHandler<TClass, T> handler( ptr );
			static PropertyInitializer intializer = {
			.m_name = name,
			.m_type = TypeInfo::GetStaticTypeInfo<T>(),
			.m_handler = handler
			};
			static Property property( typeInfo, intializer );
		}
	}
};