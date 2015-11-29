#pragma once

#include "common.h"
#include <memory>
#include <vector>
#include <stack>

class KeyValue
{
public:
	virtual const String& GetString( ) const = 0;
	virtual int GetInt( ) const = 0;
	virtual float GetFloat( ) const = 0;

	virtual const String& GetKey( ) const = 0;

	virtual void SetNext( std::shared_ptr<KeyValue> pNext ) = 0;
	virtual void SetChild( std::shared_ptr<KeyValue> pChild ) = 0;

	virtual std::shared_ptr<KeyValue>& GetNext( ) = 0;
	virtual std::shared_ptr<KeyValue>& GetChild( ) = 0;

	virtual void SetKey( const String& key ) = 0;
	virtual void SetValue( const String& value ) = 0;
};

class KeyValueImpl : public KeyValue
{
public:
	virtual const String& GetString( ) const;
	virtual int GetInt( ) const;
	virtual float GetFloat( ) const;

	virtual const String& GetKey( ) const;

	virtual void SetNext( std::shared_ptr<KeyValue> pNext );
	virtual void SetChild( std::shared_ptr<KeyValue> pChild );

	virtual std::shared_ptr<KeyValue>& GetNext( );
	virtual std::shared_ptr<KeyValue>& GetChild( );

	virtual void SetKey( const String& key );
	virtual void SetValue( const String& value );

	KeyValueImpl( );

private:
	std::shared_ptr<KeyValue> m_pNext;
	std::shared_ptr<KeyValue> m_pChild;

	String m_key;
	String m_value;
};

class ENGINE_DLL CKeyValueIterator
{
public:
	explicit CKeyValueIterator( std::shared_ptr<KeyValue> keyValue );

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
	virtual CKeyValueIterator FindKeyValue( const String& key ) = 0;
};

class KeyValueGroupImpl : public KeyValueGroup
{
public:
	virtual CKeyValueIterator FindKeyValue( const String& key );

	KeyValueGroupImpl( std::shared_ptr<KeyValue>& root );
private:
	CKeyValueIterator FindKeyValueInternal( const String& key, std::shared_ptr<KeyValue> keyValue );

	std::shared_ptr<KeyValue> m_pRoot;
};

class ENGINE_DLL CKeyValueReader
{
public:
	std::shared_ptr<KeyValueGroup> LoadKeyValueFromFile( String filename );

	CKeyValueReader( );
	~CKeyValueReader( );

private:
	void LoadKeyValueFromFileInternal( Ifstream& file, std::shared_ptr<KeyValue> keyValue );
	void PrintKeyValueInternal( std::shared_ptr<KeyValue> keyValue, int depth = 0 ) const;
};