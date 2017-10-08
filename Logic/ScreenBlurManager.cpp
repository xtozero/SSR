#include "stdafx.h"
#include "ScreenBlurManager.h"

#include "../RenderCore/CommonRenderer/IMaterial.h"
#include "../RenderCore/CommonRenderer/IRenderer.h"
#include "../RenderCore/CommonRenderer/IRenderResource.h"
#include "../RenderCore/CommonRenderer/IRenderResourceManager.h"
#include "../RenderCore/CommonRenderer/IRenderView.h"

#include "../RenderCore/IMesh.h"
#include "../RenderCore/IMeshBuilder.h"

#include "../Shared/Util.h"

bool ScreenBlurManager::Init( IRenderer& renderer, IMeshBuilder& meshBuilder )
{
	constexpr TCHAR* BLUR_MATERIAL_NAME[] = { _T( "mat_gaussian_blur_x" ), _T( "mat_gaussian_blur_y" ) };

	for ( int i = 0; i < 2; ++i )
	{
		m_pBlurMaterial[i] = renderer.GetMaterialPtr( BLUR_MATERIAL_NAME[i] );

		if ( m_pBlurMaterial[i] == nullptr )
		{
			return false;
		}
	}

	// Create Screen Rect Mesh
	meshBuilder.Clear( );

	meshBuilder.Append( MeshVertex( CXMFLOAT3( -1.f, -1.f, 1.f ), CXMFLOAT2( 0.f, 1.f ) ) );
	meshBuilder.Append( MeshVertex( CXMFLOAT3( -1.f, 1.f, 1.f ), CXMFLOAT2( 0.f, 0.f ) ) );
	meshBuilder.Append( MeshVertex( CXMFLOAT3( 1.f, -1.f, 1.f ), CXMFLOAT2( 1.f, 1.f ) ) );
	meshBuilder.Append( MeshVertex( CXMFLOAT3( 1.f, 1.f, 1.f ), CXMFLOAT2( 1.f, 0.f ) ) );

	meshBuilder.AppendIndex( 0 );
	meshBuilder.AppendIndex( 1 );
	meshBuilder.AppendIndex( 2 );
	meshBuilder.AppendIndex( 3 );

	m_pScreenRect = meshBuilder.Build( renderer, _T( "ScreenRect" ), D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );

	if ( m_pScreenRect == nullptr )
	{
		return false;
	}

	String blurTempTextureName( _T( "BlurTempTexture" ) );
	IResourceManager& resourceMgr = renderer.GetResourceManager( );

	ITexture* ssrTex = resourceMgr.CreateTexture2D( blurTempTextureName, blurTempTextureName );
	if ( ssrTex == nullptr )
	{
		return false;
	}

	m_pBlurRt = resourceMgr.CreateRenderTarget( ssrTex, blurTempTextureName );
	if ( m_pBlurRt == nullptr )
	{
		return false;
	}

	m_pBlurSrv = resourceMgr.CreateShaderResource( ssrTex, blurTempTextureName );
	if ( m_pBlurSrv == nullptr )
	{
		return false;
	}

	return true;
}

void ScreenBlurManager::Process( IRenderer& renderer, IRenderResource& destSRV, IRenderResource& destRT ) const
{
	// Set RenderTarget
	renderer.SetRenderTarget( m_pBlurRt, nullptr );

	// Set Gaussian_X Shader State
	renderer.PSSetShaderResource( 1, &destSRV );
	m_pBlurMaterial[0]->SetShader( );

	m_pScreenRect->SetMaterial( m_pBlurMaterial[0] );
	m_pScreenRect->Draw( renderer );

	// Set RenderTarget
	renderer.SetRenderTarget( &destRT, nullptr );

	// Set Gaussian_Y Shader State
	renderer.PSSetShaderResource( 1, m_pBlurSrv );
	m_pBlurMaterial[1]->SetShader( );

	m_pScreenRect->SetMaterial( m_pBlurMaterial[1] );
	m_pScreenRect->Draw( renderer );
}
