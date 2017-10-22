#include "stdafx.h"
#include "ConVar.h"
#include "KeyValueReader.h"

#include "../Shared/Util.h"

#include <fstream>
#include <iostream>
#include <tchar.h>

namespace
{
	constexpr int MAX_STRING_LEN = 256;

	ConVar( print_debug_keyValue, "0", "print data when keyValue loaded" );
}

const String& KeyValue::GetValue( ) const
{
	return m_value;
}

const String& KeyValue::GetKey( ) const
{
	return m_key;
}

void KeyValue::SetNext( std::unique_ptr<KeyValue>&& pNext )
{
	m_pNext = std::move( pNext );
}

void KeyValue::SetChild( std::unique_ptr<KeyValue>&& pChild )
{
	m_pChild = std::move( pChild );
}

void KeyValue::SetChild( std::nullptr_t pChild )
{
	m_pChild = nullptr;
}

KeyValue* KeyValue::GetNext( ) const
{
	return m_pNext.get();
}

KeyValue* KeyValue::GetChild( ) const
{
	return m_pChild.get();
}

void KeyValue::SetKey( const String& key )
{
	m_key = key;
}

void KeyValue::SetValue( const String& value )
{
	m_value = value;
}

const KeyValue* KeyValue::Find( const String& key ) const
{
	std::stack<const KeyValue*> m_openList;

	if ( m_pChild )
	{
		m_openList.push( m_pChild.get() );
	}

	const KeyValue* candidate = nullptr;

	while ( m_openList.empty( ) == false )
	{
		candidate = m_openList.top( );
		m_openList.pop( );

		if ( candidate->GetKey( ) == key )
		{
			return candidate;
		}

		if ( candidate->GetChild( ) )
		{
			m_openList.push( candidate->GetChild( ) );
		}

		if ( candidate->GetNext( ) )
		{
			m_openList.push( candidate->GetNext( ) );
		}
	}

	return nullptr;
}

CKeyValueIterator KeyValueGroupImpl::FindKeyValue( const String& key ) const
{
	return FindKeyValueInternal( key, m_pRoot.get() );
}

CKeyValueIterator KeyValueGroupImpl::FindKeyValueInternal( const String& key, KeyValue* keyValue ) const
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

CKeyValueIterator::CKeyValueIterator( KeyValue* keyValue ) :
m_current( keyValue )
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
			m_openList.push( m_current->GetNext( ) );
		}

		if ( m_current->GetChild( ) != nullptr )
		{
			m_openList.push( m_current->GetChild( ) );
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

KeyValueGroupImpl::KeyValueGroupImpl( std::unique_ptr<KeyValue>&& root ) :
m_pRoot( std::move( root ) )
{
}

std::unique_ptr<KeyValue> CKeyValueReader::LoadKeyValueFromFile( String filename )
{
	Ifstream file;

	file.open( filename );

	if ( file.is_open( ) )
	{
		std::unique_ptr<KeyValue> root = std::make_unique<KeyValue>( );
		LoadKeyValueFromFileInternal( file, root.get( ) );

		if ( print_debug_keyValue.GetBool( ) )
		{
			PrintKeyValueInternal( root.get( ) );
		}

		file.close( );
		return root;
	}

	return nullptr;
}

void CKeyValueReader::LoadKeyValueFromFileInternal( Ifstream& file, KeyValue* keyValue )
{
	KeyValue* pKeyValue = keyValue;

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
				pKeyValue->SetChild( std::make_unique<KeyValue>() );
				LoadKeyValueFromFileInternal( file, pKeyValue->GetChild( ) );
				FIX_ME( remove unnecessary new );
				if ( pKeyValue->GetChild( ) && pKeyValue->GetChild( )->GetKey( ).length() == 0 )
				{
					pKeyValue->SetChild( nullptr );
				}
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
						pKeyValue->SetNext( std::make_unique<KeyValue>( ) );
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

void CKeyValueReader::PrintKeyValueInternal( KeyValue* keyValue, int depth ) const
{
	if ( keyValue == nullptr )
	{
		return;
	}

	for ( int i = 0; i < depth; ++i )
	{
		DebugMsg( "\t" );
	}

	DebugMsg( "[ %s, %s ]\n", keyValue->GetKey( ).c_str( ), keyValue->GetValue( ).c_str( ) );

	if ( keyValue->GetChild( ) != nullptr )
	{
		PrintKeyValueInternal( keyValue->GetChild( ), depth + 1 );
	}

	if ( keyValue->GetNext( ) != nullptr )
	{
		PrintKeyValueInternal( keyValue->GetNext( ), depth );
	}
}