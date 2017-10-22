#include "stdafx.h"
#include "RenderEffect.h"

#include "CommonRenderer/IRenderer.h"
#include "CommonRenderer/IRenderResource.h"
#include "CommonRenderer/IRenderResourceManager.h"

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
	virtual void SceneBegin( IRenderer& renderer ) override;

	CEffectOrenNayar( );
private:
	bool lookupTexCreated;
};

void CEffectOrenNayar::SceneBegin( IRenderer& renderer )
{
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

		RESOURCE_INIT_DATA initData{ lookup, sizeof( float ) * lookupSize, 0 };

		IResourceManager& resourceMgr = renderer.GetResourceManager( );

		ITexture* pTexture = resourceMgr.CreateTexture2D( OREN_NAYAR_TEX_NAME, OREN_NAYAR_TEX_NAME, &initData );
		if ( pTexture )
		{
			auto lookupResource = resourceMgr.CreateShaderResource( *pTexture, OREN_NAYAR_TEX_NAME );
			renderer.PSSetShaderResource( 1, lookupResource );
#ifdef _DEBUG
			renderer.TakeSnapshot2D( OREN_NAYAR_TEX_NAME, OREN_NAYAR_SNAPSHOT_NAME );
#endif
		}

		delete[] lookup;
		lookupTexCreated = true;
	}
}

CEffectOrenNayar::CEffectOrenNayar( ) :
	lookupTexCreated( false ) {}

void CRenderEffect::SceneBegin( IRenderer& renderer )
{
	for ( auto& effect : m_renderEffects )
	{
		effect->SceneBegin( renderer );
	}
}

void CRenderEffect::SceneEnd( IRenderer& renderer )
{
	for ( auto& effect : m_renderEffects )
	{
		effect->SceneEnd( renderer );
	}
}

CRenderEffect::CRenderEffect( )
{
	m_renderEffects.emplace_back( std::make_unique<CEffectOrenNayar>( ) );
}