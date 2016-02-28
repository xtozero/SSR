#include "stdafx.h"
#include "KeyValueReader.h"

#include <cctype>
#include "../Shared/Util.h"
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

CKeyValueIterator KeyValueGroupImpl::FindKeyValue( const String& key )
{
	return FindKeyValueInternal( key, m_pRoot );
}

CKeyValueIterator KeyValueGroupImpl::FindKeyValueInternal( const String& key, std::shared_ptr<KeyValue> keyValue )
{
	if ( keyValue )
	{
		//Search Sibling
		for ( auto pKey = keyValue; pKey != nullptr; pKey = pKey->GetNext( ) )
		{
			if ( pKey->GetKey( ) == key )
			{
				return CKeyValueIterator( pKey );
			}
		}

		//Search Chaild
		if ( keyValue->GetChild( ) )
		{
			return FindKeyValueInternal( key, keyValue->GetChild( ) );
		}
	}

	return CKeyValueIterator( nullptr );
}

CKeyValueIterator::CKeyValueIterator( std::shared_ptr<KeyValue> keyValue ) :
m_current( keyValue.get() )
{
}

KeyValue* CKeyValueIterator::operator->( ) const
{
	return m_current;
}

CKeyValueIterator& CKeyValueIterator::operator++( )
{
	if ( m_current != nullptr )
	{
		if ( m_current->GetNext( ) != nullptr )
		{
			m_openList.push( m_current->GetNext( ).get() );
		}

		if ( m_current->GetChild( ) != nullptr )
		{
			m_openList.push( m_current->GetChild( ).get() );
		}

		if ( m_openList.empty( ) )
		{
			m_current = nullptr;
		}
		else
		{
			m_current = m_openList.top( );
			m_openList.pop( );
		}
	}

	return *this;
}

CKeyValueIterator CKeyValueIterator::operator++( int )
{
	CKeyValueIterator temp = *this;

	++( *this );
	return temp;
}

bool CKeyValueIterator::operator==( const void* const rhs ) const
{
	return m_current == rhs;
}

bool CKeyValueIterator::operator!=( const void* const rhs ) const
{
	return m_current != rhs;
}

bool operator==( const void* const lhs, const CKeyValueIterator& rhs )
{
	return rhs == lhs;
}

bool operator!=( const void* const lhs, const CKeyValueIterator& rhs )
{
	return rhs != lhs;
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
		
		file.close( );
		return std::make_shared<KeyValueGroupImpl>( root );
	}

	return nullptr;
}

void CKeyValueReader::LoadKeyValueFromFileInternal( Ifstream& file, std::shared_ptr<KeyValue> keyValue )
{
	std::shared_ptr<KeyValue> pKeyValue = keyValue;

	bool alreadyReadValue = false;

	while ( file.good( ) )
	{
		String buffer;

		std::getline( file, buffer );

		int curIdx = 0;
		int len = buffer.length( );

		while ( curIdx < len )
		{
			if ( buffer[curIdx] == '{' )
			{
				alreadyReadValue = true;
				pKeyValue->SetChild( std::make_shared<KeyValueImpl>( ) );
				LoadKeyValueFromFileInternal( file, pKeyValue->GetChild( ) );
				++curIdx;
			}
			else if ( buffer[curIdx] == '}' )
			{
				return;
			}
			else if ( buffer[curIdx] == '"' )
			{
				std::vector<String> params;

				UTIL::SplitByBracket( &buffer[curIdx], params, _T('"'), _T('"') );

				for ( auto i = params.begin( ); i != params.end( ); ++i )
				{
					if ( alreadyReadValue )
					{
						pKeyValue->SetNext( std::make_shared<KeyValueImpl>( ) );
						pKeyValue = pKeyValue->GetNext( );
						alreadyReadValue = false;
					}

					pKeyValue->SetKey( *i );

					if ( ++i != params.end() )
					{
						pKeyValue->SetValue( *i );
						alreadyReadValue = true;
					}
					else
					{
						break;
					}
				}

				++curIdx;
				if ( params.size( ) > 0 )
				{
					auto end = _tcsrchr( &buffer[curIdx], '"' );
					curIdx += end - &buffer[curIdx];
				}
			}
			else if ( _tcsncmp( &buffer[curIdx], _T( "//" ), _tcslen( _T( "//" ) ) ) == 0 )
			{
				break;
			}
			else
			{
				++curIdx;
			}
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
		DebugMsg( "\t" );
	}

	DebugMsg( "[ %s, %s ]\n", keyValue->GetKey( ).c_str( ), keyValue->GetString( ).c_str( ) );

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