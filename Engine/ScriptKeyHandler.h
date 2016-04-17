#pragma once

#include "common.h"

#include <functional>
#include <map>
#include <memory>
#include <vector>

class KeyValue;

template<typename T>
class CScriptKeyHandler
{
	typedef std::function<void( T*, const String&, const std::shared_ptr<KeyValue>& )> KeyHandlerFunc;

public:
	CScriptKeyHandler( ) = default;
	virtual ~CScriptKeyHandler( ) = default;

protected:
	void RegisterHandler( const String& key, KeyHandlerFunc handler );
	void Handle( const String& key, const std::shared_ptr<KeyValue>& keyValue );

private:
	std::map<String, KeyHandlerFunc> m_scriptHandler;
};

template<typename T>
void CScriptKeyHandler<T>::RegisterHandler( const String& key, KeyHandlerFunc handler )
{
	if ( m_scriptHandler.find( key ) == m_scriptHandler.end( ) )
	{
		if ( handler )
		{
			m_scriptHandler.emplace( key, handler );
		}
	}
}

template<typename T>
void CScriptKeyHandler<T>::Handle( const String& key, const std::shared_ptr<KeyValue>& keyValue )
{
	auto handler = m_scriptHandler.find( key );

	if ( handler != m_scriptHandler.end( ) )
	{
		handler->second( static_cast<T*>(this), key, keyValue );
	}
}