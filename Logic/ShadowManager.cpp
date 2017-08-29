#include "stdafx.h"
#include "LightManager.h"
#include "ShadowManager.h"

#include "../shared/Math/CXMFloat.h"
#include "../Shared/Util.h"
#include "../RenderCore/ConstantBufferDefine.h"
#include "../RenderCore/IMaterial.h"
#include "../RenderCore/IRenderer.h"
#include "../RenderCore/IRendererShadowManager.h"

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

void CShadowManager::Init( IRenderer& renderer )
{
	m_isEnabled = false;

	//�׸��ڿ� ��� ���� ����
	if ( renderer.CreateConstantBuffer( SHADOWMAP_CONST_BUFFER_NAME, sizeof( CXMFLOAT4X4 ), 2, nullptr ) == nullptr )
	{
		return;
	}

	//�׸��ڿ� ���� Ÿ�� ����
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
	//�׸��� �������� ����� �������� View ����� ����� ����
	LightViewProjection* buffer = static_cast<LightViewProjection*>( renderer.MapConstantBuffer( SHADOWMAP_CONST_BUFFER_NAME ) );

	if ( buffer )
	{
		buffer->m_lightView = XMMatrixTranspose( lightMgr.GetPrimaryLightViewMatrix( ) );
		buffer->m_lightProjection = XMMatrixTranspose( lightMgr.GerPrimaryLightProjectionMatrix( ) );

		renderer.UnMapConstantBuffer( SHADOWMAP_CONST_BUFFER_NAME );
		renderer.SetConstantBuffer( SHADOWMAP_CONST_BUFFER_NAME, static_cast<int>(VS_CONSTANT_BUFFER::LIGHT_VIEW_PROJECTION), SHADER_TYPE::VS );
	}

	//�׸��� �ؽ��ķ� ���� Ÿ�� ����
	m_renderShadowMgr->SceneBegin( renderer );

	//����Ʈ ����
	renderer.PopViewPort( );
	renderer.PopScissorRect( );
	renderer.PushViewPort( 0, 0, 2048, 2048 );
}

void CShadowManager::DrawScene( CLightManager& lightMgr, IRenderer& renderer, std::vector<std::shared_ptr<CGameObject>>& gameObjects )
{
	//�׸��� ������ ���׸���� ��ü ������Ʈ�� ������
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
	//����Ʈ ���� ����
	renderer.PopViewPort( );

	//���� Ÿ�� ���� ����, �׸��� �� ����
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
