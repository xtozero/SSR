#include "stdafx.h"
#include "KeyValueReader.h"

#include <cctype>
#include <fstream>
#include <iostream>
#include <tchar.h>

namespace
{
	const int MAX_STRING_LEN = 256;
}

const String& KeyValueImpl::GetString( ) const
{
	return m_value;
}

int KeyValueImpl::GetInt( ) const
{
	return _tstoi( m_value.c_str( ) );
}

float KeyValueImpl::GetFloat( ) const
{
	return static_cast<float>( _tstof( m_value.c_str( ) ) );
}

const String& KeyValueImpl::GetKey( ) const
{
	return m_key;
}

void KeyValueImpl::SetNext( std::shared_ptr<KeyValue> pNext )
{
	m_pNext = pNext;
}

void KeyValueImpl::SetChild( std::shared_ptr<KeyValue> pChild )
{
	m_pChild = pChild;
}

std::shared_ptr<KeyValue>& KeyValueImpl::GetNext( )
{
	return m_pNext;
}

std::shared_ptr<KeyValue>& KeyValueImpl::GetChild( )
{
	return m_pChild;
}

void KeyValueImpl::SetKey( const String& key )
{
	m_key = key;
}

void KeyValueImpl::SetValue( const String& value )
{
	m_value = value;
}

KeyValueImpl::KeyValueImpl( ) :
m_pNext( nullptr ),
m_pChild( nullptr  )
{

}

std::shared_ptr<KeyValue> KeyValueGroupImpl::FindKeyValue( const String& key )
{
	return FindKeyValueInternal( key, m_pRoot );
}

std::shared_ptr<KeyValue> KeyValueGroupImpl::FindKeyValueInternal( const String& key, std::shared_ptr<KeyValue> keyValue )
{
	if ( keyValue )
	{
		//Search Sibling
		for ( auto pKey = keyValue; pKey->GetNext( ) != nullptr; pKey = pKey->GetNext( ) )
		{
			if ( pKey->GetKey( ) == key )
			{
				return pKey;
			}
		}

		//Search Chaild
		if ( keyValue->GetChild( ) )
		{
			return FindKeyValueInternal( key, keyValue->GetChild( ) );
		}
	}

	return nullptr;
}

KeyValueGroupImpl::KeyValueGroupImpl( std::shared_ptr<KeyValue>& root ) :
m_pRoot( root )
{

}

std::shared_ptr<KeyValueGroup> CKeyValueReader::LoadKeyValueFromFile( String filename )
{
	Ifstream file;

	file.open( filename );

	if ( file.is_open( ) )
	{
		std::shared_ptr<KeyValue> root = std::make_shared<KeyValueImpl>( );
		LoadKeyValueFromFileInternal( file, root );
		PrintKeyValueInternal( root );
		
		return std::make_shared<KeyValueGroupImpl>( root );
	}

	return nullptr;
}

void CKeyValueReader::LoadKeyValueFromFileInternal( Ifstream& file, std::shared_ptr<KeyValue> keyValue )
{
	bool readValue = false;
	bool alreadyReadValue = false;
	std::shared_ptr<KeyValue> pKeyValue = keyValue;

	while ( file.good( ) )
	{
		_TCHAR buffer[MAX_STRING_LEN];

		file >> buffer;

		if ( buffer[0] == '{' )
		{
			readValue = false;
			alreadyReadValue = true;
			pKeyValue->SetChild( std::make_shared<KeyValueImpl>( ) );
			LoadKeyValueFromFileInternal( file, pKeyValue->GetChild( ) );
		}
		else if ( buffer[0] == '}' )
		{
			return;
		}
		else if ( buffer[0] == '"' )
		{
			int length = _tcslen( buffer );

			if ( length > 2 )
			{
				if ( alreadyReadValue )
				{
					pKeyValue->SetNext( std::make_shared<KeyValueImpl>( ) );
					pKeyValue = pKeyValue->GetNext( );
					alreadyReadValue = false;
				}

				String str;
				str.append( buffer, 1, length - 2 );

				if ( readValue )
				{
					pKeyValue->SetValue( str );
					alreadyReadValue = true;
				}
				else
				{
					pKeyValue->SetKey( str );
				}
			}
			
			readValue = !readValue;
		}
	}
}

void CKeyValueReader::PrintKeyValueInternal( std::shared_ptr<KeyValue> keyValue, int depth ) const
{
	if ( keyValue == nullptr )
	{
		return;
	}

	for ( int i = 0; i < depth; ++i )
	{
		Cout << "\t";
	}

	Cout << "[ " << keyValue->GetKey( ) << ", " << keyValue->GetString( ) << " ]" << std::endl;

	if ( keyValue->GetChild( ) != nullptr )
	{
		PrintKeyValueInternal( keyValue->GetChild( ), depth + 1 );
	}

	if ( keyValue->GetNext( ) != nullptr )
	{
		PrintKeyValueInternal( keyValue->GetNext( ), depth );
	}
}

CKeyValueReader::CKeyValueReader( )
{
}


CKeyValueReader::~CKeyValueReader( )
{
}
