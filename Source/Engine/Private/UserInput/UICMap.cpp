#include "stdafx.h"
#include "UserInput/UICMap.h"

#include "DataStructure/EnumStringMap.h"
#include "FileSystem/EngineFileSystem.h"
#include "Json/json.hpp"

#include <algorithm>

void UICMap::Initialize( )
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

USER_INPUT_CODE UICMap::Convert( unsigned long code )
{
	auto found = std::lower_bound( m_codeMap.begin( ), m_codeMap.end( ), code,
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

bool UICMap::LoadConfig( const char* fileName )
{
	IFileSystem* fileSystem = GetInterface<IFileSystem>( );
	FileHandle uicAsset = fileSystem->OpenFile( fileName );

	if ( uicAsset.IsValid( ) == false )
	{
		return false;
	}

	unsigned long fileSize = fileSystem->GetFileSize( uicAsset );
	char* buffer = new char[fileSize];

	IFileSystem::IOCompletionCallback ParseUICAsset;
	ParseUICAsset.BindFunctor(
		[this, uicAsset]( const char* buffer, unsigned long bufferSize )
		{
			LoadKeyCode( buffer, static_cast<size_t>( bufferSize ) );

			delete[] buffer;
			GetInterface<IFileSystem>( )->CloseFile( uicAsset );
		}
	);

	bool result = fileSystem->ReadAsync( uicAsset, buffer, fileSize, &ParseUICAsset );
	if ( result == false )
	{
		delete[] buffer;
		GetInterface<IFileSystem>( )->CloseFile( uicAsset );
	}

	return result;
}

void UICMap::LoadKeyCode( const char* uicAsset, size_t assetSize )
{
	JSON::Value root( JSON::DataType::EMPTY );
	JSON::Reader reader;

	if ( reader.Parse( uicAsset, assetSize, root ) )
	{
		IEnumStringMap* enumStringMap = GetInterface<IEnumStringMap>( );

		if ( const JSON::Value* pKeyCodes = root.Find( "KeyCodes" ) )
		{
			std::vector<const char*> members = pKeyCodes->GetMemberNames( );
			for ( const char* member : members )
			{
				if ( const JSON::Value* pUserInputCode = pKeyCodes->Find( member ) )
				{
					m_codeMap.emplace_back( static_cast<unsigned long>( enumStringMap->GetEnum( member, -1 ) ),
						static_cast<USER_INPUT_CODE>( enumStringMap->GetEnum( pUserInputCode->AsString( ), -1 ) ) );
				}
			}
		}

		std::sort( m_codeMap.begin( ), m_codeMap.end( ),
			[]( const CodePair& lhs, const CodePair& rhs )
		{
			return lhs.first < rhs.first;
		} );
	}
}