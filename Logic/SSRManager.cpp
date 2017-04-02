#include "stdafx.h"
#include "GameObject.h"
#include "SSRManager.h"

#include "../RenderCore/IBuffer.h"
#include "../RenderCore/IMesh.h"
#include "../RenderCore/IMeshBuilder.h"
#include "../RenderCore/IRenderer.h"
#include "../RenderCore/IRenderResourceManager.h"
#include "../RenderCore/IRenderState.h"
#include "../RenderCore/IRenderTarget.h"
#include "../RenderCore/IRenderView.h"
#include "../RenderCore/ITexture.h"

#include "../Shared/Util.h"

using namespace DirectX;

bool CSSRManager::Init( IRenderer& renderer, IMeshBuilder& meshBuilder )
{
	// Create Constant Buffer
	m_ssrConstantBuffer = renderer.CreateConstantBuffer( _T( "ssrConstantBuffer" ), sizeof( CXMFLOAT4X4 ), 1, nullptr );

	if ( m_ssrConstantBuffer == nullptr )
	{
		return false;
	}

	// Create Screen Space Reflect Rendertarget
	String ssrTextureName( _T( "ScreenSpaceReflect" ) );
	ITextureManager& textureMgr = renderer.GetTextureManager( );
	IRenderTargetManager& rendertargetMgr = renderer.GetRenderTargetManager( );
	IShaderResourceManager& shaderResourceMgr = renderer.GetShaderResourceManager( );

	ITexture* ssrTex = textureMgr.CreateTexture2D( renderer.GetDevice( ), ssrTextureName, ssrTextureName );
	m_pSsrRt = rendertargetMgr.CreateRenderTarget( renderer.GetDevice( ), ssrTex, nullptr, ssrTextureName );
	m_pSsrSrv = shaderResourceMgr.CreateShaderResource( renderer.GetDevice( ), ssrTex, nullptr, ssrTextureName );

	m_pDefaultRt = rendertargetMgr.FindRenderTarget( _T( "DefaultRenderTarget" ) );
	m_pDefaultDS = rendertargetMgr.FindDepthStencil( _T( "DefaultDepthStencil" ) );
	m_pDefaultSrv = shaderResourceMgr.FindShaderResource( _T( "DuplicateFrameBuffer" ) );
	m_pDepthSrv = shaderResourceMgr.FindShaderResource( _T( "DuplicateDepthGBuffer" ) );

	if ( m_pSsrRt == nullptr || m_pSsrSrv == nullptr || m_pDefaultRt == nullptr ||
		m_pDefaultDS == nullptr || m_pDefaultSrv == nullptr || m_pDepthSrv == nullptr )
	{
		return false;
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

	// Create SSR Material
	m_pSsrMaterial = renderer.GetMaterialPtr( _T( "mat_screen_space_reflect" ) );

	if ( m_pSsrMaterial == nullptr )
	{
		return false;
	}

	// Create SSR Blend Material
	m_pSsrBlendMaterial = renderer.GetMaterialPtr( _T( "mat_ssr_blend" ) );

	if ( m_pSsrBlendMaterial == nullptr )
	{
		return false;
	}

	if ( !m_blur.Init( renderer, meshBuilder ) )
	{
		return false;
	}

	return true;
}

void CSSRManager::Process( IRenderer& renderer, const std::list<CGameObject*>& reflectableList ) const
{
	// Set Constant Buffer
	if ( IRenderView* pView = renderer.GetCurrentRenderView( ) )
	{
		CXMFLOAT4X4* pSsrConstant = static_cast<CXMFLOAT4X4*>(m_ssrConstantBuffer->LockBuffer( renderer.GetDeviceContext( ) ));

		if ( pSsrConstant )
		{
			*pSsrConstant = XMMatrixTranspose( pView->GetProjectionMatrix( ) );

			m_ssrConstantBuffer->UnLockBuffer( renderer.GetDeviceContext( ) );
			m_ssrConstantBuffer->SetPSBuffer( renderer.GetDeviceContext( ), 3 );
		}
	}

	// Set RenderTarget
	IRenderTargetManager& rendertargetMgr = renderer.GetRenderTargetManager( );
	rendertargetMgr.SetRenderTarget( renderer.GetDeviceContext(), m_pSsrRt, m_pDefaultDS );

	// Clear RenderTarget to black
	m_pSsrRt->Clear( renderer.GetDeviceContext(), { 0.f, 0.f, 0.f, 0.f } );

	// Set DefaultRenderTarget By Texture
	m_pSsrMaterial->SetTexture( renderer.GetDeviceContext( ), SHADER_TYPE::PS, 1, m_pDefaultSrv );
	m_pSsrMaterial->SetTexture( renderer.GetDeviceContext( ), SHADER_TYPE::PS, 2, m_pDepthSrv );
	m_pSsrMaterial->SetShader( renderer.GetDeviceContext( ) );

	// Render Reflectable GameObject by SSR Material
	for ( auto& object : reflectableList )
	{
		object->SetOverrideMaterial( m_pSsrMaterial );
		object->UpdateWorldMatrix( renderer );
		object->Render( renderer );
		object->SetOverrideMaterial( nullptr );
	}

	m_blur.Process( renderer, *m_pSsrSrv, *m_pSsrRt );

	// Set Framebuffer RenderTarget
	rendertargetMgr.SetRenderTarget( renderer.GetDeviceContext(), m_pDefaultRt, nullptr );

	// Set Reflect Result By Texture
	m_pSsrBlendMaterial->SetTexture( renderer.GetDeviceContext( ), SHADER_TYPE::PS, 1, m_pSsrSrv );

	// Blend Result
	m_pScreenRect->SetMaterial( m_pSsrBlendMaterial );
	renderer.DrawModel( m_pScreenRect );

	m_pSsrBlendMaterial->SetTexture( renderer.GetDeviceContext( ), SHADER_TYPE::PS, 1, nullptr );
}

CSSRManager::CSSRManager( ) :
	m_pScreenRect( nullptr ),
	m_pSsrMaterial( nullptr ),
	m_pSsrBlendMaterial( nullptr ),
	m_pSsrRt( nullptr ),
	m_pDefaultRt( nullptr ),
	m_pDefaultDS( nullptr ),
	m_pSsrSrv( nullptr ),
	m_pDefaultSrv( nullptr ),
	m_pDepthSrv( nullptr ),
	m_ssrConstantBuffer( nullptr )
{
}
