#include "stdafx.h"
#include "SSRManager.h"

#include "GameLogic.h"
#include "GameObject.h"
#include "Model/IMesh.h"
#include "Model/IModelBuilder.h"

#include "../RenderCore/CommonRenderer/IMaterial.h"
#include "../RenderCore/CommonRenderer/IRenderer.h"
#include "../RenderCore/CommonRenderer/IRenderResourceManager.h"

#include "../Shared/Util.h"

using namespace DirectX;

void CSSRManager::OnDeviceRestore( CGameLogic& gameLogic )
{
	CreateDeviceDependendResource( gameLogic );
}

bool CSSRManager::Init( CGameLogic& gameLogic )
{
	return CreateDeviceDependendResource( gameLogic );
}

void CSSRManager::Process( CGameLogic& gameLogic, const std::list<CGameObject*>& reflectableList ) const
{
	IRenderer& renderer = gameLogic.GetRenderer( );

	// Set Constant Buffer
	CXMFLOAT4X4* pSsrConstant = static_cast<CXMFLOAT4X4*>( renderer.LockBuffer( m_ssrConstantBuffer ) );

	if ( pSsrConstant )
	{
		CRenderView& view = gameLogic.GetView( );
		*pSsrConstant = XMMatrixTranspose( view.GetProjectionMatrix( ) );

		renderer.UnLockBuffer( m_ssrConstantBuffer );
		renderer.BindConstantBuffer( SHADER_TYPE::PS, 3, 1, &m_ssrConstantBuffer );
	}

	// Set RenderTarget
	renderer.BindRenderTargets( &m_ssrRt, 1, m_defaultDS );

	// Clear RenderTarget to black
	renderer.ClearRendertarget( m_ssrRt, { 0.f, 0.f, 0.f, 0.f } );

	// Set DefaultRenderTarget By Texture
	renderer.BindShaderResource( SHADER_TYPE::PS, 1, 1, &m_defaultSrv );
	renderer.BindShaderResource( SHADER_TYPE::PS, 2, 1, &m_depthSrv );
	m_pSsrMaterial->Bind( renderer );

	// Render Reflectable GameObject by SSR Material
	for ( auto& object : reflectableList )
	{
		object->SetOverrideMaterial( m_pSsrMaterial );
		object->UpdateTransform( gameLogic );
		object->Render( gameLogic );
		object->SetOverrideMaterial( nullptr );
	}

	m_blur.Process( gameLogic, m_ssrSrv, m_ssrRt );

	// Set Framebuffer RenderTarget
	RE_HANDLE default[] = { RE_HANDLE_TYPE::INVALID_HANDLE };
	renderer.BindRenderTargets( &m_defaultRt, 1, default[0] );

	// Set Reflect Result By Texture
	renderer.BindShaderResource( SHADER_TYPE::PS, 1, 1, &m_ssrSrv );

	// Blend Result
	m_pScreenRect->SetMaterial( m_pSsrBlendMaterial );
	m_pScreenRect->Draw( gameLogic );

	renderer.BindShaderResource( SHADER_TYPE::PS, 1, 1, default );
}

void CSSRManager::AppSizeChanged( CGameLogic& gameLogic )
{
	CreateAppSizeDependentResource( gameLogic.GetRenderer() );
	m_blur.AppSizeChanged( gameLogic );
}

bool CSSRManager::CreateAppSizeDependentResource( IRenderer& renderer )
{
	// Create Screen Space Reflect Rendertarget
	String ssrTextureName( _T( "ScreenSpaceReflect" ) );
	IResourceManager& resourceMgr = renderer.GetResourceManager( );

	m_ssrTexture = resourceMgr.CreateTexture2D( ssrTextureName, ssrTextureName );
	if ( m_ssrTexture == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		return false;
	}

	m_ssrRt = resourceMgr.CreateRenderTarget( m_ssrTexture, ssrTextureName );
	m_ssrSrv = resourceMgr.CreateTextureShaderResource( m_ssrTexture, ssrTextureName );

	m_defaultRt = resourceMgr.FindRenderTarget( _T( "DefaultRenderTarget" ) );
	m_defaultDS = resourceMgr.FindDepthStencil( _T( "DefaultDepthStencil" ) );
	m_defaultSrv = resourceMgr.FindShaderResource( _T( "DuplicateFrameBuffer" ) );
	m_depthSrv = resourceMgr.FindShaderResource( _T( "DuplicateDepthGBuffer" ) );

	if ( m_ssrRt == RE_HANDLE_TYPE::INVALID_HANDLE ||
		m_ssrSrv == RE_HANDLE_TYPE::INVALID_HANDLE ||
		m_defaultRt == RE_HANDLE_TYPE::INVALID_HANDLE ||
		m_defaultDS == RE_HANDLE_TYPE::INVALID_HANDLE ||
		m_defaultSrv == RE_HANDLE_TYPE::INVALID_HANDLE ||
		m_depthSrv == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		return false;
	}

	return true;
}

bool CSSRManager::CreateDeviceDependendResource( CGameLogic& gameLogic )
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

	IRenderer& renderer = gameLogic.GetRenderer( );
	m_ssrConstantBuffer = renderer.CreateBuffer( trait );

	if ( m_ssrConstantBuffer == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		return false;
	}

	// Create Screen Rect Mesh
	IModelBuilder& meshBuilder = gameLogic.GetModelManager( ).GetModelBuilder( );
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
	m_pSsrMaterial = renderer.SearchMaterial( _T( "mat_screen_space_reflect" ) );
	if ( m_pSsrMaterial == nullptr )
	{
		return false;
	}

	// Create SSR Blend Material
	m_pSsrBlendMaterial = renderer.SearchMaterial( _T( "mat_ssr_blend" ) );
	if ( m_pSsrBlendMaterial == nullptr )
	{
		return false;
	}

	if ( m_blur.Init( gameLogic ) == false )
	{
		return false;
	}

	if ( CreateAppSizeDependentResource( gameLogic.GetRenderer( ) ) == false )
	{
		return false;
	}

	return true;
}
