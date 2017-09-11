#include "stdafx.h"

#include "common.h"

#include "Texture.h"
#include "TextureManager.h"

#include "../Engine/EnumStringMap.h"
#include "../Engine/KeyValueReader.h"
#include "../shared/Util.h"

#include <D3DX11.h>
#include <DXGI.h>

namespace
{
	constexpr TCHAR* DEFAULT_TEXTURE_FILE_PATH = _T( "../Texture/" );
	constexpr TCHAR* TEXTURE_DESC_SCRIPT_FILE_NAME = _T( "../Script/TextureDesc.txt" );
	constexpr TCHAR* FRAME_BUFFER_SIZE_KEYWORD = _T( "FRAME_BUFFER" );

	D3D11_TEXTURE2D_DESC gTexture2DDesc;

	namespace TEXTURE2D
	{
		void SizeHandler( CTextureManager* pTextureManager, const String&, const KeyValue* keyValue )
		{
			if ( keyValue )
			{
				if ( keyValue->GetValue() == FRAME_BUFFER_SIZE_KEYWORD )
				{
					const std::pair<int, int>& size = pTextureManager->GetFrameBufferSize( );
					assert( size.first > 0 && size.second > 0 );
					gTexture2DDesc.Width = size.first;
					gTexture2DDesc.Height = size.second;
				}
				else
				{
					Stringstream sStream( keyValue->GetValue( ) );
					sStream >> gTexture2DDesc.Width >> gTexture2DDesc.Height;
					assert( gTexture2DDesc.Width > 0 && gTexture2DDesc.Height > 0 );
				}
			}
		}

		void SubResourceHandler( CTextureManager*, const String&, const KeyValue* keyValue )
		{
			if ( keyValue )
			{
				Stringstream sStream( keyValue->GetValue( ) );
				sStream >> gTexture2DDesc.MipLevels >> gTexture2DDesc.ArraySize;
			}
		}
		
		void FormatHandler( CTextureManager*, const String&, const KeyValue* keyValue )
		{
			if ( keyValue )
			{
				gTexture2DDesc.Format = static_cast<DXGI_FORMAT>( GetEnumStringMap().GetEnum( keyValue->GetValue(), DXGI_FORMAT_UNKNOWN ) );
			}
		}

		void SampleDescHandler( CTextureManager*, const String&, const KeyValue* keyValue )
		{
			if ( keyValue )
			{
				Stringstream sStream( keyValue->GetValue( ) );
				sStream >> gTexture2DDesc.SampleDesc.Count >> gTexture2DDesc.SampleDesc.Quality;
			}
		}

		void FlagsHandler( CTextureManager*, const String&, const KeyValue* keyValue )
		{
			if ( keyValue )
			{
				Stringstream sStream( keyValue->GetValue( ) );
				
				String usage, bindFlag, cpuFlag, miscFlag;

				sStream >> usage >> bindFlag >> cpuFlag >> miscFlag;

				gTexture2DDesc.Usage = static_cast<D3D11_USAGE>( GetEnumStringMap().GetEnum( usage, D3D11_USAGE_DEFAULT ) );

				std::vector<String> bindFlags;
				UTIL::Split( bindFlag, bindFlags, _T( '|' ) );

				for ( const auto& flag : bindFlags )
				{
					gTexture2DDesc.BindFlags |= static_cast<UINT>( GetEnumStringMap( ).GetEnum( flag, D3D11_BIND_SHADER_RESOURCE ) );
				}

				gTexture2DDesc.CPUAccessFlags = static_cast<UINT>( GetEnumStringMap( ).GetEnum( cpuFlag, 0 ) );

				std::vector<String> miscFlags;
				UTIL::Split( miscFlag, miscFlags, _T( '|' ) );

				for ( const auto& flag : miscFlags )
				{
					gTexture2DDesc.MiscFlags |= static_cast<UINT>(GetEnumStringMap( ).GetEnum( flag, 0 ));
				}
			}
		}
	}
}

bool CTextureManager::LoadTexture( ID3D11Device * pDevice )
{
	return LoadTextureFromScript( pDevice, TEXTURE_DESC_SCRIPT_FILE_NAME );
}

bool CTextureManager::LoadTextureFromFile( ID3D11Device* pDevice, const String& fileName )
{
	if ( pDevice == nullptr )
	{
		return false;
	}

	TCHAR pPath[MAX_PATH];
	::GetCurrentDirectory( MAX_PATH, pPath );
	::SetCurrentDirectory( DEFAULT_TEXTURE_FILE_PATH );

	D3DX11_IMAGE_INFO info;
	HRESULT hr;
	D3DX11GetImageInfoFromFile( fileName.c_str( ), nullptr, &info, &hr );

	std::unique_ptr<ITexture> newTexture = nullptr;

	if ( SUCCEEDED( hr ) )
	{
		switch ( info.ResourceDimension )
		{
		case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
			break;
		case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
			newTexture = std::make_unique<CTexture2D>( );
			break;
		case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
			break;
		}

		if ( newTexture && newTexture->LoadFromFile( pDevice, fileName ) )
		{
			m_pTextures.emplace( fileName, std::move( newTexture ) );
			::SetCurrentDirectory( pPath );
			return true;
		}
	}

	::SetCurrentDirectory( pPath );
	return false;
}

bool CTextureManager::LoadTextureFromScript( ID3D11Device * pDevice, const String& fileName )
{
	CKeyValueReader keyValueReader;
	std::shared_ptr<KeyValueGroup> keyValue = keyValueReader.LoadKeyValueFromFile( fileName );
	
	if ( LoadTextureFromScriptInternal( pDevice, keyValue.get() ) )
	{
		return true;
	}

	return false;
}

ITexture * CTextureManager::CreateTexture2D( ID3D11Device * pDevice, const TextureDescription& desc, const String & textureName, const D3D11_SUBRESOURCE_DATA * pInitialData )
{
	if ( FindTexture( textureName ) )
	{
		DebugWarning( "CTextureManager Error - Try Create Exist Texture Name\n" );
		return nullptr;
	}

	if ( pDevice )
	{
		std::unique_ptr<CTexture2D> newTexture = std::make_unique<CTexture2D>();
		if ( newTexture->Create( pDevice, desc, pInitialData ) )
		{
			CTexture2D* ret = newTexture.get( );
			m_pTextures.emplace( textureName, std::move( newTexture ) );
			return ret;
		}
	}

	return nullptr;
}

ITexture* CTextureManager::CreateTexture2D( ID3D11Device * pDevice, const String& descName, const String & textureName, const D3D11_SUBRESOURCE_DATA * pInitialData )
{
	if ( FindTexture( textureName ) )
	{
		DebugWarning( "CTextureManager Error - Try Create Exist Texture Name\n" );
		return nullptr;
	}

	auto found = m_texture2DDesc.find( descName );

	if ( found != m_texture2DDesc.end( ) )
	{
		return CreateTexture2D( pDevice, found->second, textureName, pInitialData );
	}

	return nullptr;
}

bool CTextureManager::RegisterTexture2D( const String& textureName, Microsoft::WRL::ComPtr<ID3D11Resource> pTexture )
{
	if ( FindTexture( textureName ) )
	{
		DebugWarning( "CTextureManager Error - Try Regist Exist Texture Name\n" );
		return false;
	}

	std::unique_ptr<CTexture2D> newTexture = std::make_unique<CTexture2D>();
	newTexture->SetTexture( pTexture );
	m_pTextures.emplace( textureName, std::move( newTexture ) );
	return true;
}

ITexture* CTextureManager::FindTexture( const String& textureName ) const
{
	auto found = m_pTextures.find( textureName );

	if ( found != m_pTextures.end( ) )
	{
		return found->second.get();
	}

	return nullptr;
}

void CTextureManager::SetFrameBufferSize( UINT nWndWidth, UINT nWndHeight )
{
	m_frameBufferSize.first = nWndWidth;
	m_frameBufferSize.second = nWndHeight;
}

CTextureManager::CTextureManager( ) :
m_frameBufferSize( 0, 0 )
{
	//Texture 2D Handler
	RegisterHandler( _T( "Size" ), TEXTURE2D::SizeHandler );
	RegisterHandler( _T( "SubResourceDesc" ), TEXTURE2D::SubResourceHandler );
	RegisterHandler( _T( "Format" ), TEXTURE2D::FormatHandler );
	RegisterHandler( _T( "SampleDesc" ), TEXTURE2D::SampleDescHandler );
	RegisterHandler( _T( "Flags" ), TEXTURE2D::FlagsHandler );
}

bool CTextureManager::LoadTextureFromScriptInternal( ID3D11Device * pDevice, const KeyValueGroup* keyValue )
{
	if ( pDevice && keyValue )
	{
		auto entryPoint = keyValue->FindKeyValue( _T( "Desc" ) );

		if ( entryPoint == nullptr )
		{
			return false;
		}

		for ( auto texture = entryPoint->GetChild( ); texture != nullptr; texture = texture->GetNext( ) )
		{
			::ZeroMemory( &gTexture2DDesc, sizeof( gTexture2DDesc ) );

			for ( auto key = texture->GetChild( ); key != nullptr; key = key->GetNext( ) )
			{
				Handle( key->GetKey( ), key );
			}

			TextureDescription newDesc;
			newDesc.SetType( static_cast<int>( TEXTURE_TYPE::TEXTURE_2D ) );
			newDesc = gTexture2DDesc;
			m_texture2DDesc.emplace( texture->GetKey( ), newDesc );
		}

		return true;
	}

	return false;
}
