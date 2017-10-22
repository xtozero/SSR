#include "stdafx.h"
#include "LightManager.h"
#include "ShadowManager.h"

#include "GameLogic.h"
#include "../shared/Math/CXMFloat.h"
#include "../Shared/Util.h"
#include "../RenderCore/CommonRenderer/ConstantBufferDefine.h"
#include "../RenderCore/CommonRenderer/IBuffer.h"
#include "../RenderCore/CommonRenderer/IMaterial.h"
#include "../RenderCore/CommonRenderer/IRenderer.h"
#include "../RenderCore/CommonRenderer/IRenderResource.h"
#include "../RenderCore/CommonRenderer/IRenderResourceManager.h"

using namespace DirectX;

namespace
{
	constexpr TCHAR* SHADOWMAP_CONST_BUFFER_NAME = _T( "ShadowMapConstBuffer" );
	constexpr TCHAR* SHADOWMAP_MATERIAL_NAME = _T( "mat_shadowMap" );

	struct LightViewProjection
	{
		CXMFLOAT4X4 m_lightView;
		CXMFLOAT4X4 m_lightProjection;
	};
}

void CShadowManager::Init( CGameLogic& gameLogic )
{
	m_isEnabled = false;
	IRenderer& renderer = gameLogic.GetRenderer( );

	//그림자용 상수 버퍼 생성
	BUFFER_TRAIT buffertrait = { sizeof( CXMFLOAT4X4 ),
								2,
								RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE,
								RESOURCE_TYPE::CONSTANT_BUFFER,
								0,
								nullptr,
								0,
								0 };

	m_cbShadow = renderer.CreateBuffer( buffertrait );

	if ( m_cbShadow == nullptr )
	{
		return;
	}

	//그림자용 렌더 타겟 생성
	IResourceManager& resourceMgr = renderer.GetResourceManager( );
	m_shadowMap = resourceMgr.CreateTexture2D( _T( "ShadowMap" ), _T( "ShadowMap" ) );
	if ( m_shadowMap == nullptr )
	{
		return;
	}

	m_rtvShadowMap = resourceMgr.CreateRenderTarget( *m_shadowMap, _T( "ShadowMap" ) );
	if ( m_rtvShadowMap == nullptr )
	{
		return;
	}

	m_srvShadowMap = resourceMgr.CreateShaderResource( *m_shadowMap, _T( "ShadowMap" ) );
	if ( m_srvShadowMap == nullptr )
	{
		return;
	}

	const ITexture* depthStencilTexture = resourceMgr.CreateTexture2D( _T( "ShadowMapDepthStencil" ), _T( "ShadowMapDepthStencil" ) );
	if ( depthStencilTexture == nullptr )
	{
		return;
	}

	TEXTURE_TRAIT texTrait = depthStencilTexture->GetTrait( );
	texTrait.m_format = RESOURCE_FORMAT::D24_UNORM_S8_UINT;

	m_dsvShadowMap = resourceMgr.CreateDepthStencil( *depthStencilTexture, _T( "ShadowMapDepthStencil" ), &texTrait );
	if ( m_dsvShadowMap == nullptr )
	{
		return;
	}

	m_shadowMapMtl = renderer.GetMaterialPtr( SHADOWMAP_MATERIAL_NAME );

	if ( m_shadowMapMtl == nullptr )
	{
		return;
	}

	m_isEnabled = true;
}

void CShadowManager::SceneBegin( CLightManager& lightMgr, CGameLogic& gameLogic )
{
	IRenderer& renderer = gameLogic.GetRenderer( );

	//그림자 렌더링에 사용할 조명으로 View 행렬을 만들어 세팅
	LightViewProjection* buffer = static_cast<LightViewProjection*>( m_cbShadow->LockBuffer() );

	if ( buffer )
	{
		buffer->m_lightView = XMMatrixTranspose( lightMgr.GetPrimaryLightViewMatrix( ) );
		buffer->m_lightProjection = XMMatrixTranspose( lightMgr.GerPrimaryLightProjectionMatrix( ) );

		m_cbShadow->UnLockBuffer( );
		m_cbShadow->SetVSBuffer( static_cast<int>( VS_CONSTANT_BUFFER::LIGHT_VIEW_PROJECTION ) );
	}

	//그림자 텍스쳐로 랜더 타겟 변경
	renderer.PSSetShaderResource( 2, nullptr );
	renderer.SetRenderTarget( m_rtvShadowMap, m_dsvShadowMap );

	constexpr float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	renderer.ClearRendertarget( *m_rtvShadowMap, clearColor );
	renderer.ClearDepthStencil( *m_dsvShadowMap, 1.0f, 0 );

	//뷰포트 세팅
	renderer.PopViewPort( );
	renderer.PopScissorRect( );
	renderer.PushViewPort( 0, 0, 2048, 2048 );
}

void CShadowManager::DrawScene( CLightManager& lightMgr, CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects )
{
	//그림자 렌더링 마테리얼로 전체 오브젝트를 랜더링
	for ( auto& object : gameObjects )
	{
		if ( object.get() && object->ShouldDrawShadow( ) )
		{
			object->SetOverrideMaterial( m_shadowMapMtl );
			object->UpdateWorldMatrix( gameLogic.GetRenderer( ) );
			object->Render( gameLogic );
			object->SetOverrideMaterial( nullptr );
		}
	}
}

void CShadowManager::SceneEnd( CLightManager& lightMgr, CGameLogic& gameLogic )
{
	IRenderer& renderer = gameLogic.GetRenderer( );

	//뷰포트 원상 복귀
	gameLogic.GetRenderer().PopViewPort( );

	//랜터 타겟 원상 복귀, 그림자 맵 세팅
	renderer.SetRenderTarget( nullptr, nullptr );
	renderer.PSSetShaderResource( 2, m_srvShadowMap );
}

void CShadowManager::Process( CLightManager& lightMgr, CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects )
{
	if ( m_isEnabled )
	{
		SceneBegin( lightMgr, gameLogic );
		DrawScene( lightMgr, gameLogic, gameObjects );
		SceneEnd( lightMgr, gameLogic );
	}
}
