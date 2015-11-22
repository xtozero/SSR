#pragma once

#include "common.h"
#include <memory>

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

class KeyValueGroup
{
public:
	virtual std::shared_ptr<KeyValue> FindKeyValue( const String& key ) = 0;
};

class KeyValueGroupImpl : public KeyValueGroup
{
public:
	virtual std::shared_ptr<KeyValue> FindKeyValue( const String& key );

	KeyValueGroupImpl( std::shared_ptr<KeyValue>& root );
private:
	std::shared_ptr<KeyValue> FindKeyValueInternal( const String& key, std::shared_ptr<KeyValue> keyValue );

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