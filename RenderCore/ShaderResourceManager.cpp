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

	std::shared_ptr<IShaderResource> newShaderResource = std::make_shared<CShaderResource>();
	if ( newShaderResource && newShaderResource->LoadShaderResource( pDevice, fileName ) )
	{
		m_shaderResources.emplace( fileName, newShaderResource );
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

void CShaderResourceManager::RegisterShaderResource( const String& resourceName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srView, int srcFlag )
{
	if ( FindShaderResource( resourceName ) )
	{
		DebugWarning( "ShaderResourceManager Error - Try Regist Exist ShaderResource Name\n" );
		return;
	}

	std::shared_ptr<IShaderResource> newShaderResource = std::make_shared<CShaderResource>( srcFlag );
	
	if ( newShaderResource )
	{
		newShaderResource->SetShaderResourceView( srView );
	}

	m_shaderResources.emplace( resourceName, newShaderResource );
}

IShaderResource* CShaderResourceManager::CreateShaderResource( ID3D11Device* pDevice, const ITexture* pTexture, const CShaderResourceViewDescription* desc, const String& resourceName, int srcFlag )
{
	if ( FindShaderResource( resourceName ) )
	{
		DebugWarning( "ShaderResourceManager Error - Try Create Exist ShaderResource Name\n" );
		return nullptr;
	}

	if ( pDevice && pTexture )
	{
		std::shared_ptr<IShaderResource> newShaderResource = std::make_shared<CShaderResource>( srcFlag );

		if ( newShaderResource->CreateShaderResource( pDevice, pTexture, desc ) )
		{
			m_shaderResources.emplace( resourceName, newShaderResource );
			return newShaderResource.get( );
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