#include "stdafx.h"
#include "LightManager.h"
#include "ShadowManager.h"

#include "../Shared/Util.h"
#include "../RenderCore/ConstantBufferDefine.h"
#include "../RenderCore/IMaterial.h"
#include "../RenderCore/IRenderer.h"
#include "../RenderCore/IRendererShadowManager.h"

#include <d3dx9math.h>

namespace
{
	constexpr TCHAR* SHADOWMAP_CONST_BUFFER_NAME = _T( "ShadowMapConstBuffer" );
	constexpr TCHAR* SHADOWMAP_MATERIAL_NAME = _T( "mat_shadowMap" );

	struct LightViewProjection
	{
		D3DXMATRIX m_lightView;
		D3DXMATRIX m_lightProjection;
	};
}

void CShadowManager::Init( IRenderer& renderer )
{
	m_isEnabled = false;

	//그림자용 상수 버퍼 생성
	if ( renderer.CreateConstantBuffer( SHADOWMAP_CONST_BUFFER_NAME, sizeof( D3DXMATRIX ), 2, nullptr ) == nullptr )
	{
		return;
	}

	//그림자용 렌더 타겟 생성
	m_renderShadowMgr = renderer.GetShadowManager( );

	if ( m_renderShadowMgr )
	{
		m_renderShadowMgr->CreateShadowMapTexture( renderer );
	}
	else
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

void CShadowManager::SceneBegin( CLightManager& lightMgr, IRenderer& renderer )
{
	//그림자 렌더링에 사용할 조명으로 View 행렬을 만들어 세팅
	D3DXMATRIX& lightViewMatrix = lightMgr.GetPrimaryLightViewMatrix();
	D3DXMATRIX& lightProjMatrix = lightMgr.GerPrimaryLightProjectionMatrix();

	LightViewProjection* buffer = static_cast<LightViewProjection*>( renderer.MapConstantBuffer( SHADOWMAP_CONST_BUFFER_NAME ) );

	if ( buffer )
	{
		D3DXMatrixTranspose( &buffer->m_lightView, &lightViewMatrix );
		D3DXMatrixTranspose( &buffer->m_lightProjection, &lightProjMatrix );
		renderer.UnMapConstantBuffer( SHADOWMAP_CONST_BUFFER_NAME );
		renderer.SetConstantBuffer( SHADOWMAP_CONST_BUFFER_NAME, static_cast<int>(VS_CONSTANT_BUFFER::LIGHT_VIEW_PROJECTION), SHADER_TYPE::VS );
	}

	//그림자 텍스쳐로 랜더 타겟 변경
	m_renderShadowMgr->SceneBegin( renderer );

	//뷰포트 세팅
	renderer.PopViewPort( );
	renderer.PopScissorRect( );
	renderer.PushViewPort( 0, 0, 2048, 2048 );
}

void CShadowManager::DrawScene( CLightManager& lightMgr, IRenderer& renderer, std::vector<std::shared_ptr<CGameObject>>& gameObjects )
{
	//그림자 렌더링 마테리얼로 전체 오브젝트를 랜더링
	for ( auto& object : gameObjects )
	{
		if ( object.get() && object->ShouldDrawShadow( ) )
		{
			object->SetOverrideMaterial( m_shadowMapMtl );
			object->UpdateWorldMatrix( renderer );
			object->Render( renderer );
			object->SetOverrideMaterial( nullptr );
		}
	}
}

void CShadowManager::SceneEnd( CLightManager& lightMgr, IRenderer& renderer )
{
	//뷰포트 원상 복귀
	renderer.PopViewPort( );

	//랜터 타겟 원상 복귀, 그림자 맵 세팅
	m_renderShadowMgr->SceneEnd( renderer );
}

void CShadowManager::Process( CLightManager& lightMgr, IRenderer& renderer, std::vector<std::shared_ptr<CGameObject>>& gameObjects )
{
	if ( m_isEnabled )
	{
		SceneBegin( lightMgr, renderer );
		DrawScene( lightMgr, renderer, gameObjects );
		SceneEnd( lightMgr, renderer );
	}
}

CShadowManager::CShadowManager( ) :
	m_isEnabled( false ),
	m_renderShadowMgr( nullptr ),
	m_shadowMapMtl( nullptr )
{
}


CShadowManager::~CShadowManager( )
{
}
