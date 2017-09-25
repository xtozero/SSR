#include "stdafx.h"
#include "LightManager.h"
#include "ShadowManager.h"

#include "../shared/Math/CXMFloat.h"
#include "../Shared/Util.h"
#include "../RenderCore/ConstantBufferDefine.h"
#include "../RenderCore/IBuffer.h"
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
	BUFFER_TRAIT trait = { sizeof( CXMFLOAT4X4 ),
							2,
							BUFFER_ACCESS_FLAG::GPU_READ | BUFFER_ACCESS_FLAG::CPU_WRITE,
							BUFFER_TYPE::CONSTANT_BUFFER,
							0,
							nullptr,
							0,
							0 };

	m_cbShadow = renderer.CreateBuffer( trait );

	if ( m_cbShadow == nullptr )
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
	LightViewProjection* buffer = static_cast<LightViewProjection*>( m_cbShadow->LockBuffer() );

	if ( buffer )
	{
		buffer->m_lightView = XMMatrixTranspose( lightMgr.GetPrimaryLightViewMatrix( ) );
		buffer->m_lightProjection = XMMatrixTranspose( lightMgr.GerPrimaryLightProjectionMatrix( ) );

		m_cbShadow->UnLockBuffer( );
		m_cbShadow->SetVSBuffer( static_cast<int>( VS_CONSTANT_BUFFER::LIGHT_VIEW_PROJECTION ) );
	}

	//�׸��� �ؽ��ķ� ���� Ÿ�� ����
	m_renderShadowMgr->SceneBegin( renderer );

	//����Ʈ ����
	renderer.PopViewPort( );
	renderer.PopScissorRect( );
	renderer.PushViewPort( 0, 0, 2048, 2048 );
}

void CShadowManager::DrawScene( CLightManager& lightMgr, IRenderer& renderer, std::vector<std::unique_ptr<CGameObject>>& gameObjects )
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

void CShadowManager::Process( CLightManager& lightMgr, IRenderer& renderer, std::vector<std::unique_ptr<CGameObject>>& gameObjects )
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
