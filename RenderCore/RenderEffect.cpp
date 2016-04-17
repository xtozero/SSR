#include "stdafx.h"
#include "RenderEffect.h"

#include "IShaderResource.h"
#include "ShaderResourceManager.h"
#include "SnapShotManager.h"
#include "TextureManager.h"

#include <array>
#include <D3D11.h>

namespace
{
	const TCHAR* OREN_NAYAR_TEX_NAME = _T( "OrenNayarLookUP" );
	const TCHAR* OREN_NAYAR_SNAPSHOT_NAME = _T( "DebugOrenNayarLookUP" );
}

class CEffectOrenNayar : public IRenderEffect
{
public:
	virtual void SceneBegin( const RenderContext& context ) override;
	virtual void SceneEnd( const RenderContext& context ) override;

	CEffectOrenNayar( );
private:
	bool lookupTexCreated;
};

void CEffectOrenNayar::SceneBegin( const RenderContext& context )
{
	if ( !lookupTexCreated )
	{
		const UINT lookupSize = 512;

		float* lookup = new float[lookupSize * lookupSize];

		for ( int i = 0; i < lookupSize; ++i )
		{
			for ( int j = 0; j < lookupSize; ++j )
			{
				float VdotN = static_cast<float>(i) / lookupSize;
				float LdotN = static_cast<float>(j) / lookupSize;
				
				VdotN *= 2.f;
				VdotN -= 1.f;
				
				LdotN *= 2.f;
				LdotN -= 1.f;

				float alpha = max( acosf( VdotN ), acosf( LdotN ) );
				float beta = min( acosf( VdotN ), acosf( LdotN ) );

				lookup[i + j * lookupSize] = sinf( alpha ) * tanf( beta );
			}
		}

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = lookup;
		initData.SysMemPitch = sizeof( float ) * lookupSize;
		initData.SysMemSlicePitch = 0;

		ITexture* pTexture = context.m_pTextureMgr->CreateTexture2D( context.m_pDevice, OREN_NAYAR_TEX_NAME, OREN_NAYAR_TEX_NAME, &initData );

		if ( pTexture )
		{
			ID3D11Texture2D* pTexture2D;

			if ( SUCCEEDED( pTexture->Get( )->QueryInterface( IID_ID3D11Texture2D, (void **)&pTexture2D ) ) )
			{
				D3D11_TEXTURE2D_DESC tex2DDesc;
				pTexture2D->GetDesc( &tex2DDesc );

				D3D11_SHADER_RESOURCE_VIEW_DESC desc;
				::ZeroMemory( &desc, sizeof( desc ) );

				desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				desc.Texture2D.MipLevels = tex2DDesc.MipLevels;
				desc.Format = tex2DDesc.Format;

				//Fix me
				context.m_pShaderResourceMgr->CreateShaderResource( context.m_pDevice, pTexture, desc, OREN_NAYAR_TEX_NAME );
				auto lookupSRV = context.m_pShaderResourceMgr->FindShaderResource( OREN_NAYAR_TEX_NAME );

				if ( lookupSRV )
				{
					ID3D11ShaderResourceView* srv = lookupSRV->Get( );
					context.m_pDeviceContext->PSSetShaderResources( 1, 1, &srv );
				}
#ifdef _DEBUG
				context.m_pSnapshotMgr->TakeSnapshot2D( context.m_pDevice, context.m_pDeviceContext, OREN_NAYAR_TEX_NAME, OREN_NAYAR_SNAPSHOT_NAME );
#endif
			}
		}

		delete[] lookup;
		lookupTexCreated = true;
	}
}

void CEffectOrenNayar::SceneEnd( const RenderContext& )
{
}

CEffectOrenNayar::CEffectOrenNayar( ) :
	lookupTexCreated( false ) {}

bool CRenderEffect::Initialize( ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, CTextureManager * pTextureMgr, CShaderResourceManager * pShaderResourceMgr, CRenderTargetManager * pRenderTargetMgr, CSnapshotManager * pSnapshotMgr )
{
	if ( pDevice && pDeviceContext && pTextureMgr && pShaderResourceMgr && pRenderTargetMgr&& pSnapshotMgr )
	{
		m_context.m_pDevice = pDevice;
		m_context.m_pDeviceContext = pDeviceContext;
		m_context.m_pTextureMgr = pTextureMgr;
		m_context.m_pShaderResourceMgr = pShaderResourceMgr;
		m_context.m_pRenderTargetMgr = pRenderTargetMgr;
		m_context.m_pSnapshotMgr = pSnapshotMgr;

		return true;
	}
	else
	{
		return false;
	}
}

void CRenderEffect::SceneBegin( )
{
	for ( auto& effect : m_renderEffects )
	{
		effect->SceneBegin( m_context );
	}
}

void CRenderEffect::SceneEnd( )
{
	for ( auto& effect : m_renderEffects )
	{
		effect->SceneEnd( m_context );
	}
}

CRenderEffect::CRenderEffect( )
{
	m_renderEffects.emplace_back( std::make_unique<CEffectOrenNayar>( ) );
}
