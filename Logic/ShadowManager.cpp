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

	//�׸��ڿ� ��� ���� ����
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

	//�׸��ڿ� ���� Ÿ�� ����
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
	renderer.PSSetShaderResource( 2, nullptr );
	renderer.SetRenderTarget( m_rtvShadowMap, m_dsvShadowMap );

	constexpr float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	renderer.ClearRendertarget( *m_rtvShadowMap, clearColor );
	renderer.ClearDepthStencil( *m_dsvShadowMap, 1.0f, 0 );

	//����Ʈ ����
	renderer.PopViewPort( );
	renderer.PopScissorRect( );
	renderer.PushViewPort( 0, 0, 2048, 2048 );
}

void CShadowManager::DrawScene( CLightManager& lightMgr, CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects )
{
	//�׸��� ������ ���׸���� ��ü ������Ʈ�� ������
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

	//����Ʈ ���� ����
	gameLogic.GetRenderer().PopViewPort( );

	//���� Ÿ�� ���� ����, �׸��� �� ����
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
