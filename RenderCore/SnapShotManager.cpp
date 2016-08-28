#include "stdafx.h"
#include "IRenderResourceManager.h"
#include "ITexture.h"
#include "SnapshotManager.h"
#include "TextureDescription.h"
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

		ITexture* pSource = m_pTextureMgr->FindTexture( sourceTextureName );

		if ( pSource == nullptr )
		{
			DebugWarning( "Snapshot Error - Source Texture is nullptr\n" );
			return false;
		}

		ITexture* pDest = m_pTextureMgr->FindTexture( destTextureName );

		if ( pDest == nullptr )
		{
			pDest = CreateCloneTexture( pDevice, pSource, destTextureName );
			if ( pDest == nullptr )
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

ITexture* CSnapshotManager::CreateCloneTexture( ID3D11Device* pDevice, const ITexture* pSourceTexture, const String& textureName )
{
	if ( pDevice && pSourceTexture )
	{
		switch ( pSourceTexture->GetType() )
		{
		case TEXTURE_TYPE::TEXTURE_1D:
			break;
		case TEXTURE_TYPE::TEXTURE_2D:
			{
				assert( pSourceTexture->GetDesc( ).GetType() == static_cast<int>( TEXTURE_TYPE::TEXTURE_2D ) );
				TextureDescription texDesc = pSourceTexture->GetDesc();
				D3D11_TEXTURE2D_DESC& desc = static_cast<D3D11_TEXTURE2D_DESC&>( texDesc );

				// Applications can't specify NULL for pInitialData when creating IMMUTABLE resources
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

				if ( ITexture* pTexture = m_pTextureMgr->CreateTexture2D( pDevice, texDesc, textureName ) )
				{
					TryCreateShaderResource( pDevice, pTexture, texDesc, textureName );
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

IShaderResource* CSnapshotManager::TryCreateShaderResource( ID3D11Device* pDevice, const ITexture* pTexture, const TextureDescription& desc, const String& textureName, int srcFlag )
{
	if ( pDevice && pTexture )
	{
		switch ( pTexture->GetType( ) )
		{
		case TEXTURE_TYPE::TEXTURE_1D:
			break;
		case TEXTURE_TYPE::TEXTURE_2D:
			{
				assert( desc.GetType() == static_cast<int>( TEXTURE_TYPE::TEXTURE_2D ) );
				const D3D11_TEXTURE2D_DESC& tex2dDesc = desc;
				CShaderResourceViewDescription srvDesc;
				D3D11_SHADER_RESOURCE_VIEW_DESC& dxSrvDesc = static_cast<D3D11_SHADER_RESOURCE_VIEW_DESC&>( srvDesc );

				dxSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				dxSrvDesc.Texture2D.MipLevels = 1;
				dxSrvDesc.Texture2D.MostDetailedMip = 0;
				dxSrvDesc.Format = TranslateSRVFormat( tex2dDesc.Format );

				if ( m_pShaderResourceMgr )
				{
					return m_pShaderResourceMgr->CreateShaderResource( pDevice, pTexture, &srvDesc, textureName, srcFlag );
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

CSnapshotManager::CSnapshotManager( ITextureManager* pTextureManager, IShaderResourceManager* pShaderResourceManager ) :
	m_pTextureMgr( pTextureManager ),
	m_pShaderResourceMgr( pShaderResourceManager )
{

}