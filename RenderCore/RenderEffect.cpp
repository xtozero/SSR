#include "stdafx.h"
#include "RenderEffect.h"

#include "IRenderer.h"
#include "IRenderResourceManager.h"
#include "IShaderResource.h"
#include "ITexture.h"
#include "SnapShotManager.h"
#include "TextureDescription.h"

#include "../Shared/Util.h"

#include <array>
#include <D3D11.h>

namespace
{
	constexpr TCHAR* OREN_NAYAR_TEX_NAME = _T( "OrenNayarLookUP" );
	constexpr TCHAR* OREN_NAYAR_SNAPSHOT_NAME = _T( "DebugOrenNayarLookUP" );
}


/////////////////////////////////////////////////////////////////////////////////////////
// CEffectOrenNayar
/////////////////////////////////////////////////////////////////////////////////////////

class CEffectOrenNayar : public IRenderEffect
{
public:
	virtual void SceneBegin( IRenderer* pRenderer ) override;

	CEffectOrenNayar( );
private:
	bool lookupTexCreated;
};

void CEffectOrenNayar::SceneBegin( IRenderer* pRenderer )
{
	if ( pRenderer == nullptr )
	{
		return;
	}

	if ( !lookupTexCreated )
	{
		constexpr UINT lookupSize = 512;

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

		ID3D11Device* pDevice = pRenderer->GetDevice( );
		ID3D11DeviceContext* pDeviceContext = pRenderer->GetDeviceContext( );
		IShaderResourceManager& shaderResourceMgr = pRenderer->GetShaderResourceManager( );
		ITextureManager& textureMgr = pRenderer->GetTextureManager( );

		if ( pDevice && pDeviceContext )
		{
			ITexture* pTexture = textureMgr.CreateTexture2D( pDevice, OREN_NAYAR_TEX_NAME, OREN_NAYAR_TEX_NAME, &initData );
			if ( pTexture )
			{
				auto lookupSRV = shaderResourceMgr.CreateShaderResource( pDevice, pTexture, nullptr, OREN_NAYAR_TEX_NAME );

				if ( lookupSRV )
				{
					ID3D11ShaderResourceView* srv = lookupSRV->Get( );
					pDeviceContext->PSSetShaderResources( 1, 1, &srv );
				}
#ifdef _DEBUG
				pRenderer->TakeSnapshot2D( OREN_NAYAR_TEX_NAME, OREN_NAYAR_SNAPSHOT_NAME );
#endif
			}
		}

		delete[] lookup;
		lookupTexCreated = true;
	}
}

CEffectOrenNayar::CEffectOrenNayar( ) :
	lookupTexCreated( false ) {}

void CRenderEffect::SceneBegin( IRenderer* pRenderer )
{
	for ( auto& effect : m_renderEffects )
	{
		effect->SceneBegin( pRenderer );
	}
}

void CRenderEffect::SceneEnd( IRenderer* pRenderer )
{
	for ( auto& effect : m_renderEffects )
	{
		effect->SceneEnd( pRenderer );
	}
}

CRenderEffect::CRenderEffect( )
{
	m_renderEffects.emplace_back( std::make_unique<CEffectOrenNayar>( ) );
}