#pragma once

#include "common.h"
#include "KeyValueReader.h"
#include "../shared/UserInput.h"

#include <algorithm>
#include <fstream>
#include <vector>

namespace
{
	inline void RegisterUicEnumString()
	{
		REGISTER_ENUM_STRING( UIC_A );
		REGISTER_ENUM_STRING( UIC_B );
		REGISTER_ENUM_STRING( UIC_C );
		REGISTER_ENUM_STRING( UIC_D );
		REGISTER_ENUM_STRING( UIC_E );
		REGISTER_ENUM_STRING( UIC_F );
		REGISTER_ENUM_STRING( UIC_G );
		REGISTER_ENUM_STRING( UIC_H );
		REGISTER_ENUM_STRING( UIC_I );
		REGISTER_ENUM_STRING( UIC_J );
		REGISTER_ENUM_STRING( UIC_K );
		REGISTER_ENUM_STRING( UIC_L );
		REGISTER_ENUM_STRING( UIC_M );
		REGISTER_ENUM_STRING( UIC_N );
		REGISTER_ENUM_STRING( UIC_O );
		REGISTER_ENUM_STRING( UIC_P );
		REGISTER_ENUM_STRING( UIC_Q );
		REGISTER_ENUM_STRING( UIC_R );
		REGISTER_ENUM_STRING( UIC_S );
		REGISTER_ENUM_STRING( UIC_T );
		REGISTER_ENUM_STRING( UIC_U );
		REGISTER_ENUM_STRING( UIC_V );
		REGISTER_ENUM_STRING( UIC_W );
		REGISTER_ENUM_STRING( UIC_X );
		REGISTER_ENUM_STRING( UIC_Y );
		REGISTER_ENUM_STRING( UIC_Z );
		REGISTER_ENUM_STRING( UIC_LEFT );
		REGISTER_ENUM_STRING( UIC_UP );
		REGISTER_ENUM_STRING( UIC_RIGHT );
		REGISTER_ENUM_STRING( UIC_DOWN );
		REGISTER_ENUM_STRING( UIC_ZERO );
		REGISTER_ENUM_STRING( UIC_ONE );
		REGISTER_ENUM_STRING( UIC_TWO );
		REGISTER_ENUM_STRING( UIC_THREE );
		REGISTER_ENUM_STRING( UIC_FOUR );
		REGISTER_ENUM_STRING( UIC_FIVE );
		REGISTER_ENUM_STRING( UIC_SIX );
		REGISTER_ENUM_STRING( UIC_SEVEN );
		REGISTER_ENUM_STRING( UIC_EIGHT );
		REGISTER_ENUM_STRING( UIC_NINE );
		REGISTER_ENUM_STRING( UIC_NUM_PAD_ZERO );
		REGISTER_ENUM_STRING( UIC_NUM_PAD_ONE );
		REGISTER_ENUM_STRING( UIC_NUM_PAD_TWO );
		REGISTER_ENUM_STRING( UIC_NUM_PAD_THREE );
		REGISTER_ENUM_STRING( UIC_NUM_PAD_FOUR );
		REGISTER_ENUM_STRING( UIC_NUM_PAD_FIVE );
		REGISTER_ENUM_STRING( UIC_NUM_PAD_SIX );
		REGISTER_ENUM_STRING( UIC_NUM_PAD_SEVEN );
		REGISTER_ENUM_STRING( UIC_NUM_PAD_EIGHT );
		REGISTER_ENUM_STRING( UIC_NUM_PAD_NINE );
		REGISTER_ENUM_STRING( UIC_MULTIPLY );
		REGISTER_ENUM_STRING( UIC_ADD );
		REGISTER_ENUM_STRING( UIC_SUBTRACT );
		REGISTER_ENUM_STRING( UIC_DECIMAL );
		REGISTER_ENUM_STRING( UIC_DIVIDE );
		REGISTER_ENUM_STRING( UIC_BACKSPACE );
		REGISTER_ENUM_STRING( UIC_TAB );
		REGISTER_ENUM_STRING( UIC_ENTER );
		REGISTER_ENUM_STRING( UIC_PAUSE );
		REGISTER_ENUM_STRING( UIC_NUMLOCK );
		REGISTER_ENUM_STRING( UIC_SCROLLLOCK );
		REGISTER_ENUM_STRING( UIC_CAPSLOCK );
		REGISTER_ENUM_STRING( UIC_ESCAPE );
		REGISTER_ENUM_STRING( UIC_SPACEBAR );
		REGISTER_ENUM_STRING( UIC_PAGEUP );
		REGISTER_ENUM_STRING( UIC_PAGEDOWN );
		REGISTER_ENUM_STRING( UIC_END );
		REGISTER_ENUM_STRING( UIC_HOME );
		REGISTER_ENUM_STRING( UIC_INSERT );
		REGISTER_ENUM_STRING( UIC_DELETE );
		REGISTER_ENUM_STRING( UIC_F1 );
		REGISTER_ENUM_STRING( UIC_F2 );
		REGISTER_ENUM_STRING( UIC_F3 );
		REGISTER_ENUM_STRING( UIC_F4 );
		REGISTER_ENUM_STRING( UIC_F5 );
		REGISTER_ENUM_STRING( UIC_F6 );
		REGISTER_ENUM_STRING( UIC_F7 );
		REGISTER_ENUM_STRING( UIC_F8 );
		REGISTER_ENUM_STRING( UIC_F9 );
		REGISTER_ENUM_STRING( UIC_F10 );
		REGISTER_ENUM_STRING( UIC_F11 );
		REGISTER_ENUM_STRING( UIC_F12 );
		REGISTER_ENUM_STRING( UIC_LEFTSHIFT );
		REGISTER_ENUM_STRING( UIC_RIGHTSHIFT );
		REGISTER_ENUM_STRING( UIC_LEFTCONTROL );
		REGISTER_ENUM_STRING( UIC_RIGHTCONTROL );
		REGISTER_ENUM_STRING( UIC_LEFTALT );
		REGISTER_ENUM_STRING( UIC_RIGHTALT );
		REGISTER_ENUM_STRING( UIC_SEMICOLON );
		REGISTER_ENUM_STRING( UIC_EQUALS );
		REGISTER_ENUM_STRING( UIC_COMMA );
		REGISTER_ENUM_STRING( UIC_UNDERSOCRE );
		REGISTER_ENUM_STRING( UIC_PERIOD );
		REGISTER_ENUM_STRING( UIC_SLASH );
		REGISTER_ENUM_STRING( UIC_TILDE );
		REGISTER_ENUM_STRING( UIC_LEFTBRACKET );
		REGISTER_ENUM_STRING( UIC_BACKSLASH );
		REGISTER_ENUM_STRING( UIC_RIGHTBRACKET );
		REGISTER_ENUM_STRING( UIC_QUOTE );
		REGISTER_ENUM_STRING( UIC_MOUSE_MOVE );
		REGISTER_ENUM_STRING( UIC_MOUSE_SCROLLUP );
		REGISTER_ENUM_STRING( UIC_MOUSE_SCROLLDOWN );
		REGISTER_ENUM_STRING( UIC_MOUSE_WHEELSPIN );
		REGISTER_ENUM_STRING( UIC_MOUSE_LEFT );
		REGISTER_ENUM_STRING( UIC_MOUSE_RIGHT );
		REGISTER_ENUM_STRING( UIC_MOUSE_MIDDLE );
	}
}

class UICMap
{
public:
	void Initialize( )
	{
		RegisterUicEnumString( );
	}

	USER_INPUT_CODE Convert( unsigned long code )
	{
		auto found = std::lower_bound( m_codeMap.begin(), m_codeMap.end(), code,
							[]( const CodePair& codePair, unsigned long code )
							{
								return codePair.first < code;
							} );

		if ( found != m_codeMap.end( ) && found->first == code )
		{
			return found->second;	
		}

		return UIC_UNKNOWN;
	}

	bool LoadConfig( const String& fileName )
	{
		CKeyValueReader cfg;
		std::unique_ptr<KeyValue> pKeyValues = cfg.LoadKeyValueFromFile( fileName );
		return LoadKeyCode( *pKeyValues );
	}

private:
	bool LoadKeyCode( const KeyValue& keyValues )
	{
		for ( KeyValue* keycode = keyValues.GetChild( ); keycode != nullptr; keycode = keycode->GetNext( ) )
		{
			m_codeMap.emplace_back( static_cast<unsigned long>( GetEnumStringMap( ).GetEnum( keycode->GetKey( ), -1 ) ),
									static_cast<USER_INPUT_CODE>( GetEnumStringMap( ).GetEnum( keycode->GetValue( ), -1 ) ) );
		}

		std::sort( m_codeMap.begin( ), m_codeMap.end( ),
					[]( const CodePair& lhs, const CodePair& rhs )
					{
						return lhs.first < rhs.first;
					} );

		return true;
	}

	using CodePair = std::pair<unsigned long, USER_INPUT_CODE>;
	std::vector<CodePair> m_codeMap;
};
