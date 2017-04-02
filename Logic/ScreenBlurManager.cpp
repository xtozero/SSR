#include "stdafx.h"
#include "ScreenBlurManager.h"

#include "../RenderCore/IMesh.h"
#include "../RenderCore/IMaterial.h"
#include "../RenderCore/IMeshBuilder.h"
#include "../RenderCore/IRenderer.h"
#include "../RenderCore/IRenderResourceManager.h"
#include "../RenderCore/IRenderTarget.h"
#include "../RenderCore/IRenderView.h"
#include "../RenderCore/ITexture.h"

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
	ITextureManager& textureMgr = renderer.GetTextureManager( );
	IRenderTargetManager& rendertargetMgr = renderer.GetRenderTargetManager( );
	IShaderResourceManager& shaderResourceMgr = renderer.GetShaderResourceManager( );

	ITexture* ssrTex = textureMgr.CreateTexture2D( renderer.GetDevice( ), blurTempTextureName, blurTempTextureName );
	if ( ssrTex == nullptr )
	{
		return false;
	}

	m_pBlurRt = rendertargetMgr.CreateRenderTarget( renderer.GetDevice( ), ssrTex, nullptr, blurTempTextureName );
	if ( m_pBlurRt == nullptr )
	{
		return false;
	}

	m_pBlurSrv = shaderResourceMgr.CreateShaderResource( renderer.GetDevice( ), ssrTex, nullptr, blurTempTextureName );
	if ( m_pBlurSrv == nullptr )
	{
		return false;
	}

	return true;
}

void ScreenBlurManager::Process( IRenderer& renderer, IShaderResource& destSRV, IRenderTarget& destRT ) const
{
	IRenderTargetManager& rendertargetMgr = renderer.GetRenderTargetManager( );

	// Set RenderTarget
	rendertargetMgr.SetRenderTarget( renderer.GetDeviceContext( ), m_pBlurRt, nullptr );

	// Set Gaussian_X Shader State
	m_pBlurMaterial[0]->SetTexture( renderer.GetDeviceContext( ), SHADER_TYPE::PS, 1, &destSRV );
	m_pBlurMaterial[0]->SetShader( renderer.GetDeviceContext( ) );

	m_pScreenRect->SetMaterial( m_pBlurMaterial[0] );
	renderer.DrawModel( m_pScreenRect );

	// Set RenderTarget
	rendertargetMgr.SetRenderTarget( renderer.GetDeviceContext( ), &destRT, nullptr );

	// Set Gaussian_Y Shader State
	m_pBlurMaterial[1]->SetTexture( renderer.GetDeviceContext( ), SHADER_TYPE::PS, 1, m_pBlurSrv );
	m_pBlurMaterial[1]->SetShader( renderer.GetDeviceContext( ) );

	m_pScreenRect->SetMaterial( m_pBlurMaterial[1] );
	renderer.DrawModel( m_pScreenRect );
}
