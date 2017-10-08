#include "stdafx.h"
#include "D3D11ResourceManager.h"

#include "D3D11DepthStencil.h"
#include "D3D11RenderTarget.h"
#include "D3D11ShaderResource.h"
#include "D3D11Texture.h"

#include "../common.h"
#include "../../Engine/EnumStringMap.h"
#include "../../Engine/KeyValueReader.h"
#include "../../shared/Util.h"

#include <D3DX11.h>
#include <DXGI.h>
#include <fstream>

namespace
{
	constexpr TCHAR* DEFAULT_TEXTURE_FILE_PATH = _T( "../Texture/" );
	constexpr TCHAR* TEXTURE_DESC_SCRIPT_FILE_NAME = _T( "../Script/TextureDesc.txt" );
	constexpr TCHAR* FRAME_BUFFER_SIZE_KEYWORD = _T( "FRAME_BUFFER" );

	void SizeHandler( const KeyValue* keyValue, const std::pair<int, int>& frameBufferSize, TEXTURE_TRAIT& trait )
	{
		if ( keyValue )
		{
			if ( keyValue->GetValue( ) == FRAME_BUFFER_SIZE_KEYWORD )
			{
				trait.m_width = frameBufferSize.first;
				trait.m_height = frameBufferSize.second;
				trait.m_depth = 1;
			}
			else
			{
				Stringstream sStream( keyValue->GetValue( ) );
				sStream >> trait.m_width >> trait.m_height >> trait.m_depth;
			}
		}
	}

	void MipLevelHandler( const KeyValue* keyValue, TEXTURE_TRAIT& trait )
	{
		if ( keyValue )
		{
			Stringstream sStream( keyValue->GetValue( ) );
			sStream >> trait.m_mipLevels;
		}
	}

	void FormatHandler( const KeyValue* keyValue, TEXTURE_TRAIT& trait )
	{
		if ( keyValue )
		{
			trait.m_format = GetEnumStringMap( ).GetEnum( keyValue->GetValue( ), RESOURCE_FORMAT::UNKNOWN );
		}
	}

	void SampleDescHandler( const KeyValue* keyValue, TEXTURE_TRAIT& trait )
	{
		if ( keyValue )
		{
			Stringstream sStream( keyValue->GetValue( ) );
			sStream >> trait.m_sampleCount >> trait.m_sampleQuality;
		}
	}

	void AccessHandler( const KeyValue* keyValue, TEXTURE_TRAIT& trait )
	{
		if ( keyValue )
		{
			std::vector<String> accessFlags;
			UTIL::Split( keyValue->GetValue( ), accessFlags, _T( '|' ) );

			for ( const auto& flag : accessFlags )
			{
				trait.m_access |= static_cast<UINT>( GetEnumStringMap( ).GetEnum( flag, 0 ) );
			}
		}
	}

	void TypeHandler( const KeyValue* keyValue, TEXTURE_TRAIT& trait )
	{
		if ( keyValue )
		{
			std::vector<String> resourceTypes;
			UTIL::Split( keyValue->GetValue( ), resourceTypes, _T( '|' ) );

			for ( const auto& type : resourceTypes )
			{
				trait.m_type |= static_cast<D3D11_USAGE>( GetEnumStringMap( ).GetEnum( type, RESOURCE_TYPE::SHADER_RESOURCE ) );
			}
		}
	}

	void MiscHandler( const KeyValue* keyValue, TEXTURE_TRAIT& trait )
	{
		if ( keyValue )
		{
			std::vector<String> miscFlags;
			UTIL::Split( keyValue->GetValue( ), miscFlags, _T( '|' ) );

			for ( const auto& flag : miscFlags )
			{
				trait.m_miscFlag |= static_cast<UINT>( GetEnumStringMap( ).GetEnum( flag, 0 ) );
			}
		}
	}
}

bool CD3D11ResourceManager::Bootup( )
{
	CKeyValueReader keyValueReader;
	std::shared_ptr<KeyValueGroup> keyValue = keyValueReader.LoadKeyValueFromFile( TEXTURE_DESC_SCRIPT_FILE_NAME );

	if ( keyValue == nullptr )
	{
		return false;
	}

	auto entryPoint = keyValue->FindKeyValue( _T( "Desc" ) );

	if ( entryPoint == nullptr )
	{
		return false;
	}

	for ( auto texture = entryPoint->GetChild( ); texture != nullptr; texture = texture->GetNext( ) )
	{
		auto ret = m_textureTraits.emplace( texture->GetKey( ), TEXTURE_TRAIT( ) );

		TEXTURE_TRAIT& newTrait = ret.first->second;

		for ( auto key = texture->GetChild( ); key != nullptr; key = key->GetNext( ) )
		{
			if ( key->GetKey( ) == _T( "Size" ) )
			{
				SizeHandler( key, m_frameBufferSize, newTrait );
			}
			else if ( key->GetKey( ) == _T( "Sample" ) )
			{
				SampleDescHandler( key, newTrait );
			}
			else if ( key->GetKey( ) == _T( "MipLevels" ) )
			{
				MipLevelHandler( key, newTrait );
			}
			else if ( key->GetKey( ) == _T( "Format" ) )
			{
				FormatHandler( key, newTrait );
			}
			else if ( key->GetKey( ) == _T( "Access" ) )
			{
				AccessHandler( key, newTrait );
			}
			else if ( key->GetKey( ) == _T( "Type" ) )
			{
				TypeHandler( key, newTrait );
			}
			else if ( key->GetKey( ) == _T( "Misc" ) )
			{
				MiscHandler( key, newTrait );
			}
		}
	}

	return true;
}

bool CD3D11ResourceManager::LoadTextureFromFile( const String& fileName )
{
	TCHAR pPath[MAX_PATH];
	::GetCurrentDirectory( MAX_PATH, pPath );
	::SetCurrentDirectory( DEFAULT_TEXTURE_FILE_PATH );

	D3DX11_IMAGE_INFO info;
	HRESULT hr;
	D3DX11GetImageInfoFromFile( fileName.c_str( ), nullptr, &info, &hr );

	if ( SUCCEEDED( hr ) )
	{
		switch ( info.ResourceDimension )
		{
		case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
		{
			std::unique_ptr<CD3D11Texture1D> newTexture = std::make_unique<CD3D11Texture1D>( );
			if ( newTexture && newTexture->LoadFromFile( m_pDevice, fileName.c_str( ) ) )
			{
				m_pTextures.emplace( fileName, std::move( newTexture ) );
			}
		}
		break;
		case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
		{
			std::unique_ptr<CD3D11Texture2D> newTexture = std::make_unique<CD3D11Texture2D>( );
			if ( newTexture && newTexture->LoadFromFile( m_pDevice, fileName.c_str( ) ) )
			{
				m_pTextures.emplace( fileName, std::move( newTexture ) );
			}
		}
		break;
		case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
		{
			std::unique_ptr<CD3D11Texture3D> newTexture = std::make_unique<CD3D11Texture3D>( );
			if ( newTexture && newTexture->LoadFromFile( m_pDevice, fileName.c_str( ) ) )
			{
				m_pTextures.emplace( fileName, std::move( newTexture ) );
			}
		}
		break;
		}
	}

	::SetCurrentDirectory( pPath );
	return false;
}

ITexture* CD3D11ResourceManager::CreateTexture1D( const TEXTURE_TRAIT& trait, const String& textureName, const RESOURCE_INIT_DATA* initData )
{
	if ( FindTexture( textureName ) )
	{
		DebugWarning( "CD3D11ResourceManager Error - Try Create Exist Texture Name\n" );
		return nullptr;
	}

	std::unique_ptr<CD3D11Texture1D> newTexture = std::make_unique<CD3D11Texture1D>( );
	if ( newTexture->Create( m_pDevice, trait, initData ) )
	{
		CD3D11Texture1D* ret = newTexture.get( );
		m_pTextures.emplace( textureName, std::move( newTexture ) );
		return ret;
	}

	return nullptr;
}

ITexture* CD3D11ResourceManager::CreateTexture1D( const String& descName, const String& textureName, const RESOURCE_INIT_DATA* initData )
{
	if ( FindTexture( textureName ) )
	{
		DebugWarning( "CD3D11ResourceManager Error - Try Create Exist Texture Name\n" );
		return nullptr;
	}

	auto found = m_textureTraits.find( descName );
	if ( found != m_textureTraits.end( ) )
	{
		return CreateTexture1D( found->second, textureName, initData );
	}

	return nullptr;
}

ITexture* CD3D11ResourceManager::CreateTexture2D( const TEXTURE_TRAIT& trait, const String& textureName, const RESOURCE_INIT_DATA* initData )
{
	if ( FindTexture( textureName ) )
	{
		DebugWarning( "CD3D11ResourceManager Error - Try Create Exist Texture Name\n" );
		return nullptr;
	}

	std::unique_ptr<CD3D11Texture2D> newTexture = std::make_unique<CD3D11Texture2D>( );
	if ( newTexture->Create( m_pDevice, trait, initData ) )
	{
		CD3D11Texture2D* ret = newTexture.get( );
		m_pTextures.emplace( textureName, std::move( newTexture ) );
		return ret;
	}

	return nullptr;
}

ITexture* CD3D11ResourceManager::CreateTexture2D( const String& descName, const String& textureName, const RESOURCE_INIT_DATA* initData )
{
	if ( FindTexture( textureName ) )
	{
		DebugWarning( "CD3D11ResourceManager Error - Try Create Exist Texture Name\n" );
		return nullptr;
	}

	auto found = m_textureTraits.find( descName );
	if ( found != m_textureTraits.end( ) )
	{
		return CreateTexture2D( found->second, textureName, initData );
	}

	return nullptr;
}

bool CD3D11ResourceManager::RegisterTexture2D( const String& textureName, void* pTexture )
{
	if ( FindTexture( textureName ) )
	{
		DebugWarning( "CTextureManager Error - Try Regist Exist Texture Name\n" );
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D11Resource> texture = static_cast<ID3D11Resource*>( pTexture );
	std::unique_ptr<CD3D11Texture2D> newTexture = std::make_unique<CD3D11Texture2D>( );
	newTexture->SetTexture( texture );
	m_pTextures.emplace( textureName, std::move( newTexture ) );
	return true;
}

ITexture* CD3D11ResourceManager::FindTexture( const String& textureName ) const
{
	auto found = m_pTextures.find( textureName );

	if ( found != m_pTextures.end( ) )
	{
		return found->second.get( );
	}

	return nullptr;
}

void CD3D11ResourceManager::SetFrameBufferSize( UINT nWndWidth, UINT nWndHeight )
{
	m_frameBufferSize.first = nWndWidth;
	m_frameBufferSize.second = nWndHeight;
}

CD3D11ResourceManager::CD3D11ResourceManager( ID3D11Device* pDevice ) :
	m_pDevice( pDevice ),
	m_frameBufferSize( 0, 0 )
{
	assert( m_pDevice );
}

IRenderResource* CD3D11ResourceManager::CreateRenderTarget( const ITexture* pTexture, const String& renderTargetName, const TEXTURE_TRAIT* trait )
{
	if ( pTexture == nullptr )
	{
		return nullptr;
	}

	if ( auto renderTarget = FindRenderTarget( renderTargetName ) )
	{
		return renderTarget;
	}

	std::unique_ptr<CRenderTarget> newRenderTarget = std::make_unique<CRenderTarget>( );

	if ( newRenderTarget->CreateRenderTarget( m_pDevice, static_cast<ID3D11Resource*>( pTexture->Get( ) ), trait ) )
	{
		CRenderTarget* ret = newRenderTarget.get( );
		m_renderTargets.emplace( renderTargetName, std::move( newRenderTarget ) );
		return ret;
	}

	return nullptr;
}

IRenderResource* CD3D11ResourceManager::CreateDepthStencil( const ITexture* pTexture, const String& depthStencilName, const TEXTURE_TRAIT* trait )
{
	if ( auto depthStencil = FindDepthStencil( depthStencilName ) )
	{
		return depthStencil;
	}

	if ( pTexture )
	{
		std::unique_ptr<CDepthStencil> newDepthStencil = std::make_unique<CDepthStencil>( );

		if ( newDepthStencil->CreateDepthStencil( m_pDevice, pTexture, trait ) )
		{
			CDepthStencil* ret = newDepthStencil.get( );
			m_depthStencils.emplace( depthStencilName, std::move( newDepthStencil ) );
			return ret;
		}
	}

	return nullptr;
}

IRenderResource* CD3D11ResourceManager::FindRenderTarget( const String& renderTargetName ) const
{
	auto found = m_renderTargets.find( renderTargetName );

	if ( found != m_renderTargets.end( ) )
	{
		return found->second.get( );
	}

	return nullptr;
}

IRenderResource* CD3D11ResourceManager::FindDepthStencil( const String& depthStencilName ) const
{
	auto found = m_depthStencils.find( depthStencilName );

	if ( found != m_depthStencils.end( ) )
	{
		return found->second.get( );
	}

	return nullptr;
}

void CD3D11ResourceManager::LoadShaderResourceFromFile( const String& fileName )
{
	TCHAR pPath[MAX_PATH];
	::GetCurrentDirectory( MAX_PATH, pPath );
	::SetCurrentDirectory( DEFAULT_TEXTURE_FILE_PATH );

	Ifstream textureFile;
	textureFile.open( fileName, 0 );

	if ( !textureFile.is_open( ) )
	{
		::SetCurrentDirectory( pPath );
		return;
	}

	std::unique_ptr<CD3D11ShaderResource> newShaderResource = std::make_unique<CD3D11ShaderResource>( );
	if ( newShaderResource->LoadShaderResourceFromFile( m_pDevice, fileName ) )
	{
		m_shaderResources.emplace( fileName, std::move( newShaderResource ) );
	}

	textureFile.close( );

	::SetCurrentDirectory( pPath );
}

IRenderResource* CD3D11ResourceManager::FindShaderResource( const String& fileName ) const
{
	auto found = m_shaderResources.find( fileName );

	if ( found != m_shaderResources.end( ) )
	{
		return found->second.get( );
	}
	else
	{
		return nullptr;
	}
}

void CD3D11ResourceManager::RegisterShaderResource( const String& resourceName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srView )
{
	if ( FindShaderResource( resourceName ) )
	{
		DebugWarning( "CD3D11ResourceManager Error - Try Regist Exist ShaderResource Name\n" );
		return;
	}

	std::unique_ptr<CD3D11ShaderResource> newShaderResource = std::make_unique<CD3D11ShaderResource>( );

	newShaderResource->SetShaderResourceView( srView );
	m_shaderResources.emplace( resourceName, std::move( newShaderResource ) );
}

IRenderResource* CD3D11ResourceManager::CreateShaderResource( const ITexture* pTexture, const String& resourceName, const TEXTURE_TRAIT* trait )
{
	if ( FindShaderResource( resourceName ) )
	{
		DebugWarning( "CD3D11ResourceManager Error - Try Create Exist ShaderResource Name\n" );
		return nullptr;
	}

	std::unique_ptr<CD3D11ShaderResource> newShaderResource = std::make_unique<CD3D11ShaderResource>( );

	if ( newShaderResource->CreateShaderResource( m_pDevice, pTexture, trait ) )
	{
		CD3D11ShaderResource* ret = newShaderResource.get( );
		m_shaderResources.emplace( resourceName, std::move( newShaderResource ) );
		return ret;
	}

	return nullptr;
}

ITexture* CD3D11ResourceManager::CreateCloneTexture( const ITexture* pSourceTexture, const String& textureName )
{
	if ( pSourceTexture )
	{
		switch ( pSourceTexture->GetType( ) )
		{
		case TEXTURE_TYPE::TEXTURE_1D:
			break;
		case TEXTURE_TYPE::TEXTURE_2D:
		{
			TEXTURE_TRAIT trait = pSourceTexture->GetTrait( );

			// Applications can't specify NULL for pInitialData when creating IMMUTABLE resources
			trait.m_type |= RESOURCE_TYPE::SHADER_RESOURCE;
			trait.m_access = RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE;

			if ( ITexture* pTexture = CreateTexture2D( trait, textureName ) )
			{
				trait.m_format = ( trait.m_format == RESOURCE_FORMAT::R24G8_TYPELESS ) ? RESOURCE_FORMAT::R24_UNORM_X8_TYPELESS : trait.m_format;

				CreateShaderResource( pTexture, textureName, &trait );

				return pTexture;
			}
		}
		break;
		case TEXTURE_TYPE::TEXTURE_3D:
			break;
		default:
			break;
		}
	}

	return nullptr;
}

void CD3D11ResourceManager::TakeSnapshot( ID3D11DeviceContext* pDeviceContext, const String& sourceTextureName, const String& destTextureName )
{
	ITexture* pSource = FindTexture( sourceTextureName );

	if ( pSource == nullptr )
	{
		DebugWarning( "Snapshot Error - Source Texture is nullptr\n" );
	}

	ITexture* pDest = FindTexture( destTextureName );

	if ( pDest == nullptr )
	{
		pDest = CreateCloneTexture( pSource, destTextureName );
		if ( pDest == nullptr )
		{
			DebugWarning( "Snapshot Error - Fail Create DestTexture \n" );
		}
	}

	pDeviceContext->CopyResource( reinterpret_cast<ID3D11Resource*>( pDest->Get( ) ), reinterpret_cast<ID3D11Resource*>( pSource->Get( ) ) );
}
