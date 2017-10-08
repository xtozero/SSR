#include "stdafx.h"
#include "SSRManager.h"

#include "GameObject.h"

#include "../RenderCore/CommonRenderer/IBuffer.h"
#include "../RenderCore/CommonRenderer/IRenderer.h"
#include "../RenderCore/CommonRenderer/IRenderResource.h"
#include "../RenderCore/CommonRenderer/IRenderResourceManager.h"
#include "../RenderCore/CommonRenderer/IRenderState.h"
#include "../RenderCore/CommonRenderer/IRenderView.h"

#include "../RenderCore/IMesh.h"
#include "../RenderCore/IMeshBuilder.h"

#include "../Shared/Util.h"

using namespace DirectX;

bool CSSRManager::Init( IRenderer& renderer, IMeshBuilder& meshBuilder )
{
	// Create Constant Buffer
	BUFFER_TRAIT trait = { sizeof( CXMFLOAT4X4 ), 
							1,
							RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE,
							RESOURCE_TYPE::CONSTANT_BUFFER,
							0,
							nullptr,
							0,
							0 };

	m_ssrConstantBuffer = renderer.CreateBuffer( trait );

	if ( m_ssrConstantBuffer == nullptr )
	{
		return false;
	}

	// Create Screen Space Reflect Rendertarget
	String ssrTextureName( _T( "ScreenSpaceReflect" ) );
	IResourceManager& resourceMgr = renderer.GetResourceManager( );

	ITexture* ssrTex = resourceMgr.CreateTexture2D( ssrTextureName, ssrTextureName );
	m_pSsrRt = resourceMgr.CreateRenderTarget( ssrTex, ssrTextureName );
	m_pSsrSrv = resourceMgr.CreateShaderResource( ssrTex, ssrTextureName );

	m_pDefaultRt = resourceMgr.FindRenderTarget( _T( "DefaultRenderTarget" ) );
	m_pDefaultDS = resourceMgr.FindDepthStencil( _T( "DefaultDepthStencil" ) );
	m_pDefaultSrv = resourceMgr.FindShaderResource( _T( "DuplicateFrameBuffer" ) );
	m_pDepthSrv = resourceMgr.FindShaderResource( _T( "DuplicateDepthGBuffer" ) );

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
		CXMFLOAT4X4* pSsrConstant = static_cast<CXMFLOAT4X4*>(m_ssrConstantBuffer->LockBuffer( ));

		if ( pSsrConstant )
		{
			*pSsrConstant = XMMatrixTranspose( pView->GetProjectionMatrix( ) );

			m_ssrConstantBuffer->UnLockBuffer( );
			m_ssrConstantBuffer->SetPSBuffer( 3 );
		}
	}

	// Set RenderTarget
	renderer.SetRenderTarget( m_pSsrRt, m_pDefaultDS );

	// Clear RenderTarget to black
	renderer.ClearRendertarget( *m_pSsrRt, { 0.f, 0.f, 0.f, 0.f } );

	// Set DefaultRenderTarget By Texture
	renderer.PSSetShaderResource( 1, m_pDefaultSrv );
	renderer.PSSetShaderResource( 2, m_pDepthSrv );
	m_pSsrMaterial->SetShader( );

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
	renderer.SetRenderTarget( m_pDefaultRt, nullptr );

	// Set Reflect Result By Texture
	renderer.PSSetShaderResource( 1, m_pSsrSrv );

	// Blend Result
	m_pScreenRect->SetMaterial( m_pSsrBlendMaterial );
	m_pScreenRect->Draw( renderer );

	renderer.PSSetShaderResource( 1, nullptr );
}
