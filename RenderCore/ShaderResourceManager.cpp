#include "stdafx.h"

#include "IShaderResource.h"
#include "ITexture.h"
#include "ShaderResource.h"
#include "ShaderResourceManager.h"
#include "../shared/Util.h"

#include <d3d11.h>
#include <fstream>

namespace
{
	constexpr TCHAR* DEFAULT_TEXTURE_FILE_PATH = _T( "../Texture/" );
}

void CShaderResourceManager::LoadShaderResourceFromFile( ID3D11Device* pDevice, const String& fileName )
{
	if ( pDevice == nullptr )
	{
		return;
	}

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

	std::unique_ptr<CShaderResource> newShaderResource = std::make_unique<CShaderResource>();
	if ( newShaderResource->LoadShaderResource( pDevice, fileName ) )
	{
		m_shaderResources.emplace( fileName, std::move( newShaderResource ) );
	}

	textureFile.close( );

	::SetCurrentDirectory( pPath );
}

IShaderResource* CShaderResourceManager::FindShaderResource( const String& fileName ) const
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

void CShaderResourceManager::RegisterShaderResource( const String& resourceName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srView )
{
	if ( FindShaderResource( resourceName ) )
	{
		DebugWarning( "ShaderResourceManager Error - Try Regist Exist ShaderResource Name\n" );
		return;
	}

	std::unique_ptr<CShaderResource> newShaderResource = std::make_unique<CShaderResource>();
	
	newShaderResource->SetShaderResourceView( srView );
	m_shaderResources.emplace( resourceName, std::move( newShaderResource ) );
}

IShaderResource* CShaderResourceManager::CreateShaderResource( ID3D11Device* pDevice, const ITexture* pTexture, const CShaderResourceViewDescription* desc, const String& resourceName )
{
	if ( FindShaderResource( resourceName ) )
	{
		DebugWarning( "ShaderResourceManager Error - Try Create Exist ShaderResource Name\n" );
		return nullptr;
	}

	if ( pDevice && pTexture )
	{
		std::unique_ptr<CShaderResource> newShaderResource = std::make_unique<CShaderResource>( );

		if ( newShaderResource->CreateShaderResource( pDevice, pTexture, desc ) )
		{
			CShaderResource* ret = newShaderResource.get();
			m_shaderResources.emplace( resourceName, std::move( newShaderResource ) );
			return ret;
		}
	}

	return nullptr;
}

CShaderResourceManager::CShaderResourceManager( )
{
}


CShaderResourceManager::~CShaderResourceManager( )
{
}
