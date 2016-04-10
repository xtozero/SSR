#include "stdafx.h"
#include "ITexture.h"
#include "ShaderResourceManager.h"
#include "SnapshotManager.h"
#include "TextureManager.h"
#include "../shared/Util.h"

#include <D3D11.h>

namespace
{
	DXGI_FORMAT TranslateSRVFormat( DXGI_FORMAT format )
	{
		switch ( format )
		{
		case DXGI_FORMAT_R24G8_TYPELESS:
			return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
			break;
		default:
			break;
		}

		return format;
	}
}

bool CSnapshotManager::TakeSnapshot2D( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const String& sourceTextureName, const String& destTextureName )
{
	if ( pDevice && pDeviceContext )
	{
		if ( m_pTextureMgr == nullptr )
		{
			DebugWarning( "Snapshot Error - TextureManager is not registed\n" );
			return false;
		}

		std::shared_ptr<ITexture> pSource = m_pTextureMgr->FindTexture( sourceTextureName );

		if ( pSource.get( ) == nullptr )
		{
			DebugWarning( "Snapshot Error - Source Texture is nullptr\n" );
			return false;
		}

		std::shared_ptr<ITexture> pDest = m_pTextureMgr->FindTexture( destTextureName );

		if ( pDest.get( ) == nullptr )
		{
			pDest = CreateCloneTexture( pDevice, pSource, destTextureName );
			if ( pDest.get( ) == nullptr )
			{
				DebugWarning( "Snapshot Error - Fail Create DestTexture \n" );
				return false;
			}
		}

		pDeviceContext->CopyResource( pDest->Get(), pSource->Get( ) );
		return true;
	}

	return false;
}

std::shared_ptr<ITexture> CSnapshotManager::CreateCloneTexture( ID3D11Device* pDevice, std::shared_ptr<ITexture>& pSourceTexture, const String& textureName )
{
	if ( pDevice && pSourceTexture.get( ) )
	{
		switch ( pSourceTexture->GetType() )
		{
		case TEXTURE_TYPE::TEXTURE_1D:
			break;
		case TEXTURE_TYPE::TEXTURE_2D:
			{
				ID3D11Texture2D* pTexture2D;

				if ( SUCCEEDED( pSourceTexture->Get( )->QueryInterface( IID_ID3D11Texture2D, (void **)&pTexture2D ) ) )
				{
					D3D11_TEXTURE2D_DESC desc;

					pTexture2D->GetDesc( &desc );

					if ( m_pTextureMgr->CreateTexture2D( pDevice, desc, textureName ) )
					{
						std::shared_ptr<ITexture> pTexture = m_pTextureMgr->FindTexture( textureName );
						TryCreateShaderResource( pDevice, pTexture, desc, textureName );
						return pTexture;
					}
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

bool CSnapshotManager::TryCreateShaderResource( ID3D11Device* pDevice, std::shared_ptr<ITexture>& pTexture, const D3D11_TEXTURE2D_DESC& desc, const String& textureName, int srcFlag )
{
	if ( pDevice && pTexture.get( ) )
	{
		switch ( pTexture->GetType( ) )
		{
		case TEXTURE_TYPE::TEXTURE_1D:
			break;
		case TEXTURE_TYPE::TEXTURE_2D:
		{
			ID3D11Texture2D* pTexture2D;

			if ( SUCCEEDED( pTexture->Get( )->QueryInterface( IID_ID3D11Texture2D, (void **)&pTexture2D ) ) )
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
				::ZeroMemory( &srvDesc, sizeof( D3D11_SHADER_RESOURCE_VIEW_DESC ) );

				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MipLevels = 1;
				srvDesc.Texture2D.MostDetailedMip = 0;
				srvDesc.Format = TranslateSRVFormat( desc.Format );

				if ( m_pShaderResourceMgr )
				{
					return m_pShaderResourceMgr->CreateShaderResource( pDevice, pTexture, srvDesc, textureName, srcFlag );
				}
			}
		}
		break;
		case TEXTURE_TYPE::TEXTURE_3D:
			break;
		default:
			break;
		}
	}

	return false;
}

CSnapshotManager::CSnapshotManager( CTextureManager* pTextureManager, CShaderResourceManager* pShaderResourceManager ) :
	m_pTextureMgr( pTextureManager ),
	m_pShaderResourceMgr( pShaderResourceManager )
{

}