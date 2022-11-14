#pragma once

#include "SizedTypes.h"

#include <concepts>
#include <map>
#include <string>
#include <string_view>
#include <typeinfo>
#include <vector>

class Method;
class Property;
class TypeInfo;

template <typename T>
concept HasSuper = requires
{
	typename T::Super;
} && !std::same_as<typename T::Super, void>;

template <typename T>
concept HasStaticTypeInfo = requires
{
	T::StaticTypeInfo();
};

template <typename T, typename U = void>
struct SuperClassTypeDeduction
{
	using Type = void;
};

template <typename T>
struct SuperClassTypeDeduction<T, std::void_t<typename T::ThisType>>
{
	using Type = T::ThisType;
};

template <typename T>
struct TypeInfoInitializer
{
	TypeInfoInitializer( const char* name )
		: m_name( name )
	{
		if constexpr ( HasSuper<T> )
		{
			m_super = &( typename T::Super::StaticTypeInfo() );
		}
	}

	const char* m_name = nullptr;
	const TypeInfo* m_super = nullptr;
};

class TypeInfo
{
public:
	template <typename T>
	explicit TypeInfo( const TypeInfoInitializer<T>& initializer ) 
		: m_typeHash( typeid( T ).hash_code() )
		, m_name( initializer.m_name )
		, m_fullName( typeid( T ).name() )
		, m_super( initializer.m_super )
		, m_isArray( std::is_array_v<T> )
	{
		if constexpr ( HasSuper<T> )
		{
			CollectSuperMethods();
			CollectSuperProperties();
		}
	}

	const TypeInfo* GetSuper() const;

	template <typename T> requires HasStaticTypeInfo<T>
	static const TypeInfo& GetStaticTypeInfo()
	{
		return T::StaticTypeInfo();
	}

	template <typename T> requires
		std::is_pointer_v<T>
		&& HasStaticTypeInfo<std::remove_pointer_t<T>>
		static const TypeInfo& GetStaticTypeInfo()
	{
		return std::remove_pointer_t<T>::StaticTypeInfo();
	}

	template <typename T> requires
		( !HasStaticTypeInfo<T> ) &&
		( !HasStaticTypeInfo<std::remove_pointer_t<T>> )
		static const TypeInfo& GetStaticTypeInfo()
	{
		static TypeInfo typeInfo{ TypeInfoInitializer<T>( "unreflected_type_variable" ) };
		return typeInfo;
	}

	bool IsA( const TypeInfo& other ) const;
	bool IsChildOf( const TypeInfo& other ) const;

	template <typename T>
	bool IsA() const
	{
		return IsA( GetStaticTypeInfo<T>() );
	}

	template <typename T>
	bool IsChildOf() const
	{
		return IsChildOf( GetStaticTypeInfo<T>() );
	}

	const std::vector<const Method*>& GetMethods() const
	{
		return m_methods;
	}

	const Method* GetMethod( const char* name ) const;

	const std::vector<const Property*>& GetProperties() const
	{
		return m_properties;
	}

	const Property* GetProperty( const char* name ) const;

	const char* GetName() const
	{
		return m_name;
	}

	bool IsArray() const
	{
		return m_isArray;
	}

private:
	void CollectSuperMethods();
	void CollectSuperProperties();

	friend Method;
	friend Property;
	using MethodMap = std::map<std::string_view, const Method*>;
	using PropertyMap = std::map<std::string_view, const Property*>;

	void AddProperty( const Property* property );
	void AddMethod( const Method* method );

	size_t m_typeHash;
	const char* m_name = nullptr;
	std::string m_fullName;
	const TypeInfo* m_super = nullptr;

	bool m_isArray = false;

	std::vector<const Method*> m_methods;
	MethodMap m_methodMap;

	std::vector<const Property*> m_properties;
	PropertyMap m_propertyMap;
};