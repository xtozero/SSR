#include "stdafx.h"
#include "Scene/SSRManager.h"

#include "Core/GameLogic.h"
#include "GameObject/GameObject.h"
#include "Model/IMesh.h"
#include "Model/IModelBuilder.h"
#include "Render/IRenderer.h"
#include "Render/IRenderResourceManager.h"

#include "Util.h"

using namespace DirectX;

void CSSRManager::OnDeviceRestore( CGameLogic& gameLogic )
{
	CreateDeviceDependendResource( gameLogic );
}

bool CSSRManager::Init( CGameLogic& gameLogic )
{
	return CreateDeviceDependendResource( gameLogic );
}

void CSSRManager::PreProcess( CGameLogic& gameLogic, const std::list<CGameObject*>* renderableList ) const
{
	ImUI& ui = gameLogic.GetUIManager( );
	ui.Window( "SSR Option" );
	if ( ui.Button( "SSR On/Off" ) )
	{
		m_isEnabled = !m_isEnabled;
	}

	ui.SliderFloat( "Depthbias Slider", &m_properties.depthbias, 0.f, 1.f );
	ui.SameLine( );
	ui.Text( "Depthbias" );

	ui.SliderFloat( "Ray Step Scale Slider", &m_properties.rayStepScale, 1.f, 10.f );
	ui.SameLine( );
	ui.Text( "Ray Step Scale" );

	ui.SliderFloat( "Max Ray Length Slider", &m_properties.maxRayLength, 100.f, 500.f );
	ui.SameLine( );
	ui.Text( "Max Ray Length" );

	ui.SliderInt( "Max Ray Step Slider", &m_properties.maxRayStep, 100, 300 );
	ui.SameLine( );
	ui.Text( "Max Ray Step" );

	ui.EndWindow( );

	if ( m_isEnabled == false )
	{
		return;
	}

	assert( renderableList != nullptr );
	IRenderer& renderer = gameLogic.GetRenderer( );

	renderer.ClearRendertarget( m_backfaceDepthRt, { 1.f, 1.f, 1.f, 1.f } );
	renderer.ClearDepthStencil( m_defaultDS, 0.f, 0 );
	renderer.BindRenderTargets( &m_backfaceDepthRt, 1, m_defaultDS );

	// Draw back face depth
	for ( auto& object : renderableList[OPAQUE_RENDERABLE] )
	{
		object->SetOverrideMaterial( m_backfaceDepthMaterial );
		object->UpdateTransform( gameLogic );
		object->Render( gameLogic );
		object->SetOverrideMaterial( INVALID_MATERIAL );
	}

	for ( auto& object : renderableList[TRANSPARENT_RENDERABLE] )
	{
		object->SetOverrideMaterial( m_backfaceDepthMaterial );
		object->UpdateTransform( gameLogic );
		object->Render( gameLogic );
		object->SetOverrideMaterial( INVALID_MATERIAL );
	}

	renderer.ClearDepthStencil( m_defaultDS, 1.f, 0 );
}

void CSSRManager::Process( CGameLogic& gameLogic, const std::list<CGameObject*>* renderableList ) const
{
	if ( m_isEnabled == false )
	{
		return;
	}

	assert( renderableList != nullptr );
	IRenderer& renderer = gameLogic.GetRenderer( );

	// Set Constant Buffer
	SSRConstantBuffer* pSsrConstant = static_cast<SSRConstantBuffer*>( renderer.LockBuffer( m_ssrConstantBuffer ) );

	if ( pSsrConstant )
	{
		CRenderView& view = gameLogic.GetView( );
		pSsrConstant->projectionMatrix = XMMatrixTranspose( view.GetProjectionMatrix( ) );
		pSsrConstant->properties = m_properties;

		renderer.UnLockBuffer( m_ssrConstantBuffer );
		renderer.BindConstantBuffer( SHADER_TYPE::PS, 3, 1, &m_ssrConstantBuffer );
	}

	// Set RenderTarget
	renderer.BindRenderTargets( &m_ssrRt, 1, m_defaultDS );

	// Clear RenderTarget to black
	renderer.ClearRendertarget( m_ssrRt, { 0.f, 0.f, 0.f, 0.f } );

	// Set DefaultRenderTarget By Texture
	RE_HANDLE srv[] = { m_defaultSrv, m_depthSrv, m_backfaceDepthSrv };
	renderer.BindShaderResource( SHADER_TYPE::PS, 1, 3, srv );
	renderer.BindMaterial( m_ssrMaterial );

	// Render Reflectable GameObject by SSR Material
	for ( auto& object : renderableList[REFLECT_RENDERABLE] )
	{
		object->SetOverrideMaterial( m_ssrMaterial );
		object->UpdateTransform( gameLogic );
		object->Render( gameLogic );
		object->SetOverrideMaterial( INVALID_MATERIAL );
	}

	// m_blur.Process( gameLogic, m_ssrSrv, m_ssrRt );

	// Set Framebuffer RenderTarget
	RE_HANDLE default[] = { RE_HANDLE::InValidHandle( ), RE_HANDLE::InValidHandle( ), RE_HANDLE::InValidHandle( ) };
	renderer.BindRenderTargets( &m_defaultRt, 1, default[0] );

	// Set Reflect Result By Texture
	renderer.BindShaderResource( SHADER_TYPE::PS, 1, 1, &m_ssrSrv );

	// Blend Result
	m_pScreenRect->SetMaterial( m_ssrBlendMaterial );
	m_pScreenRect->Draw( gameLogic );

	renderer.BindShaderResource( SHADER_TYPE::PS, 1, 3, default );
}

void CSSRManager::AppSizeChanged( CGameLogic& gameLogic )
{
	CreateAppSizeDependentResource( gameLogic.GetRenderer() );
	m_blur.AppSizeChanged( gameLogic );
}

bool CSSRManager::CreateAppSizeDependentResource( IRenderer& renderer )
{
	MULTISAMPLE_OPTION multiSampleOption;
	renderer.GetRendererMultiSampleOption( &multiSampleOption );

	// Create Screen Space Reflect Rendertarget
	IResourceManager& resourceMgr = renderer.GetResourceManager( );

	String ssrTextureName( _T( "ScreenSpaceReflect" ) );
	TEXTURE_TRAIT ssrTexTrait = {
		0U,
		0U,
		1U,
		static_cast<UINT>( multiSampleOption.m_count ),
		static_cast<UINT>( multiSampleOption.m_quality ),
		1U,
		RESOURCE_FORMAT::R8G8B8A8_UNORM,
		RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
		RESOURCE_BIND_TYPE::RENDER_TARGET | RESOURCE_BIND_TYPE::SHADER_RESOURCE,
		RESOURCE_MISC::APP_SIZE_DEPENDENT
	};

	m_ssrTexture = resourceMgr.CreateTexture2D( ssrTexTrait, ssrTextureName );
	if ( m_ssrTexture == RE_HANDLE::InValidHandle( ) )
	{
		return false;
	}

	m_ssrRt = resourceMgr.CreateRenderTarget( m_ssrTexture, ssrTextureName );
	m_ssrSrv = resourceMgr.CreateTextureShaderResource( m_ssrTexture, ssrTextureName );

	m_defaultRt = resourceMgr.FindRenderTarget( _T( "DefaultRenderTarget" ) );
	m_defaultDS = resourceMgr.FindDepthStencil( _T( "DefaultDepthStencil" ) );
	m_defaultSrv = resourceMgr.FindShaderResource( _T( "DuplicateFrameBuffer" ) );
	m_depthSrv = resourceMgr.FindShaderResource( _T( "DuplicateDepthGBuffer" ) );

	String backfaceDepthTexName( _T( "BackfaceDepth" ) );
	m_backfaceDepthTexture = resourceMgr.CreateTexture2D( backfaceDepthTexName, backfaceDepthTexName );
	if ( m_backfaceDepthTexture == RE_HANDLE::InValidHandle( ) )
	{
		return false;
	}

	m_backfaceDepthRt = resourceMgr.CreateRenderTarget( m_backfaceDepthTexture, backfaceDepthTexName );
	m_backfaceDepthSrv = resourceMgr.CreateTextureShaderResource( m_backfaceDepthTexture, backfaceDepthTexName );

	if ( m_ssrRt == RE_HANDLE::InValidHandle( ) ||
		m_ssrSrv == RE_HANDLE::InValidHandle( ) ||
		m_defaultRt == RE_HANDLE::InValidHandle( ) ||
		m_defaultDS == RE_HANDLE::InValidHandle( ) ||
		m_defaultSrv == RE_HANDLE::InValidHandle( ) ||
		m_depthSrv == RE_HANDLE::InValidHandle( ) ||
		m_backfaceDepthRt  == RE_HANDLE::InValidHandle( ) ||
		m_backfaceDepthSrv == RE_HANDLE::InValidHandle( ) )
	{
		return false;
	}

	return true;
}

bool CSSRManager::CreateDeviceDependendResource( CGameLogic& gameLogic )
{
	// Create Constant Buffer
	BUFFER_TRAIT trait = { sizeof( SSRConstantBuffer ),
		1,
		RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE,
		RESOURCE_BIND_TYPE::CONSTANT_BUFFER,
		0,
		nullptr,
		0,
		0 };

	IRenderer& renderer = gameLogic.GetRenderer( );
	m_ssrConstantBuffer = renderer.CreateBuffer( trait );

	if ( m_ssrConstantBuffer == RE_HANDLE::InValidHandle( ) )
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

	MULTISAMPLE_OPTION multiSampleOption;
	renderer.GetRendererMultiSampleOption( &multiSampleOption );

	// Create Thickness Material
	m_backfaceDepthMaterial = renderer.SearchMaterial( _T( "mat_draw_back_face_depth" ) );

	if ( m_backfaceDepthMaterial == INVALID_MATERIAL )
	{
		return false;
	}

	// Create SSR Material
	MULTISAMPLE_OPTION msOption;
	renderer.GetRendererMultiSampleOption( &msOption );
	if ( msOption.m_count > 1 )
	{
		m_ssrMaterial = renderer.SearchMaterial( _T( "mat_ms_screen_space_reflect" ) );
	}
	else
	{
		m_ssrMaterial = renderer.SearchMaterial( _T( "mat_screen_space_reflect" ) );
	}
	
	if ( m_ssrMaterial == INVALID_MATERIAL )
	{
		return false;
	}

	// Create SSR Blend Material
	if ( msOption.m_count > 1 )
	{
		m_ssrBlendMaterial = renderer.SearchMaterial( _T( "mat_ms_ssr_blend" ) );
	}
	else
	{
		m_ssrBlendMaterial = renderer.SearchMaterial( _T( "mat_ssr_blend" ) );
	}

	if ( m_ssrBlendMaterial == INVALID_MATERIAL )
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
