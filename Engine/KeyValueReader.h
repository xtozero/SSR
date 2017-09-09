#pragma once

#include "common.h"
#include <memory>
#include <stack>
#include <type_traits>
#include <vector>

#include "../shared/Util.h"

class ENGINE_DLL KeyValue
{
public:
	template<typename T>
	const T GetValue( ) const
	{
		static_assert(!std::is_same<T, String>::value, "Use non-template GetValue function to get string value");

		Stringstream ss( m_value );
		T value;
		ss >> value;

		return value;
	}

	const String& GetValue( ) const;

	template<typename T>
	const T GetKey( ) const
	{
		static_assert( !std::is_same<T, String>::value, "Use non-template GetKey function to get string value" );

		Stringstream ss( m_key );
		T value;
		ss >> value;

		return value;
	}

	const String& GetKey( ) const;

	void SetNext( std::unique_ptr<KeyValue>&& pNext );
	void SetChild( std::unique_ptr<KeyValue>&& pChild );
	void SetChild( std::nullptr_t pChild );

	KeyValue* GetNext( ) const;
	KeyValue* GetChild( ) const;

	void SetKey( const String& key );
	void SetValue( const String& value );

private:
	std::unique_ptr<KeyValue> m_pNext;
	std::unique_ptr<KeyValue> m_pChild;

	String m_key;
	String m_value;
};

class ENGINE_DLL CKeyValueIterator
{
public:
	explicit CKeyValueIterator( KeyValue* keyValue );

	KeyValue* operator->( ) const;
	CKeyValueIterator& operator++( );
	CKeyValueIterator operator++( int );

	bool operator==( const void* const rhs ) const;
	bool operator!=( const void* const rhs ) const;

	friend ENGINE_DLL bool operator==( const void* const lhs, const CKeyValueIterator& rhs );
	friend ENGINE_DLL bool operator!=( const void* const lhs, const CKeyValueIterator& rhs );
private:
	KeyValue* m_current;
	std::stack<KeyValue*> m_openList;
};

ENGINE_DLL bool operator==( const void* const lhs, const CKeyValueIterator& rhs );
ENGINE_DLL bool operator!=( const void* const lhs, const CKeyValueIterator& rhs );

class KeyValueGroup
{
public:
	virtual CKeyValueIterator FindKeyValue( const String& key ) const = 0;

	virtual ~KeyValueGroup( ) = default;
};

class KeyValueGroupImpl : public KeyValueGroup
{
public:
	virtual CKeyValueIterator FindKeyValue( const String& key ) const override;

	explicit KeyValueGroupImpl( std::unique_ptr<KeyValue>&& root );
	~KeyValueGroupImpl( ) = default;
private:
	CKeyValueIterator FindKeyValueInternal( const String& key, KeyValue* keyValue ) const;

	std::unique_ptr<KeyValue> m_pRoot;
};

class ENGINE_DLL CKeyValueReader
{
public:
	std::unique_ptr<KeyValueGroupImpl> LoadKeyValueFromFile( String filename );

	CKeyValueReader( ) = default;
	~CKeyValueReader( ) = default;

private:
	void LoadKeyValueFromFileInternal( Ifstream& file, KeyValue* keyValue );
	void PrintKeyValueInternal( KeyValue* keyValue, int depth = 0 ) const;
};