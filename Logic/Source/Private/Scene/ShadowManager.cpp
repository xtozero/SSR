#include "stdafx.h"
#include "Scene/ShadowManager.h"

#include "Core/GameLogic.h"
#include "GameObject/LightManager.h"
#include "Math/CXMFloat.h"
#include "Render/IRenderer.h"
#include "Render/IRenderResourceManager.h"
#include "Scene/ConstantBufferDefine.h"
#include "Util.h"

using namespace DirectX;

namespace
{
	constexpr TCHAR* SHADOWMAP_CONST_BUFFER_NAME = _T( "ShadowMapConstBuffer" );
	constexpr TCHAR* SHADOWMAP_MATERIAL_NAME = _T( "mat_shadowMap" );
}

void CShadowManager::OnDeviceRestore( CGameLogic& gameLogic )
{
	m_isEnabled = CreateDeviceDependentResource( gameLogic.GetRenderer() );
}

void CShadowManager::Init( CGameLogic& gameLogic )
{
	IRenderer& renderer = gameLogic.GetRenderer( );
	m_isEnabled = CreateDeviceDependentResource( renderer );
}

void CShadowManager::Process( CLightManager& lightMgr, CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects )
{
	if ( m_isEnabled )
	{
		IRenderer& renderer = gameLogic.GetRenderer( );

		using namespace SHARED_CONSTANT_BUFFER;
		RE_HANDLE shadowBuffer = gameLogic.GetCommonConstantBuffer( VS_SHADOW );
		//�׸��� �������� ����� �������� View ����� ����� ����
		ShadowTransform* buffer = static_cast<ShadowTransform*>( renderer.LockBuffer( shadowBuffer ) );

		if ( buffer )
		{
			buffer->m_lightView = XMMatrixTranspose( lightMgr.GetPrimaryLightViewMatrix( ) );
			buffer->m_lightProjection = XMMatrixTranspose( lightMgr.GerPrimaryLightProjectionMatrix( ) );

			renderer.UnLockBuffer( shadowBuffer );
			renderer.BindConstantBuffer( SHADER_TYPE::VS, VS_CONSTANT_BUFFER::SHADOW, 1, &shadowBuffer );
		}
		else
		{
			__debugbreak( );
		}

		//�׸��� �ؽ��ķ� ���� Ÿ�� ����
		RE_HANDLE defaultResource[] = { RE_HANDLE_TYPE::INVALID_HANDLE };
		renderer.BindShaderResource( SHADER_TYPE::PS, 2, 1, defaultResource );
		renderer.BindRenderTargets( &m_rtvShadowMap, 1, m_dsvShadowMap );

		constexpr float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		renderer.ClearRendertarget( m_rtvShadowMap, clearColor );
		renderer.ClearDepthStencil( m_dsvShadowMap, 1.0f, 0 );

		//����Ʈ ����
		CRenderView& view = gameLogic.GetView( );
		view.PopViewPort( );
		view.PushViewPort( 0, 0, 2048, 2048 );
		view.SetViewPort( renderer );
		view.SetScissorRects( renderer );
		view.UpdataView( renderer, gameLogic.GetCommonConstantBuffer( VS_VIEW_PROJECTION ) );

		//�׸��� ������ ���׸���� ��ü ������Ʈ�� ������
		for ( auto& object : gameObjects )
		{
			if ( object.get( ) && object->ShouldDrawShadow( ) )
			{
				object->SetOverrideMaterial( m_shadowMapMtl );
				object->UpdateTransform( gameLogic );
				object->Render( gameLogic );
				object->SetOverrideMaterial( INVALID_MATERIAL );
			}
		}

		//����Ʈ ���� ����
		view.PopViewPort( );
		view.PopScissorRect( );

		//���� Ÿ�� ���� ����, �׸��� �� ����
		RE_HANDLE default[] = { RE_HANDLE_TYPE::INVALID_HANDLE };
		renderer.BindRenderTargets( default, 1, default[0] );
		renderer.BindShaderResource( SHADER_TYPE::PS, 2, 1, &m_srvShadowMap );
	}
}

bool CShadowManager::CreateDeviceDependentResource( IRenderer& renderer )
{
	//�׸��ڿ� ���� Ÿ�� ����
	IResourceManager& resourceMgr = renderer.GetResourceManager( );
	m_shadowMap = resourceMgr.CreateTexture2D( _T( "ShadowMap" ), _T( "ShadowMap" ) );
	if ( m_shadowMap == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		return false;
	}

	m_rtvShadowMap = resourceMgr.CreateRenderTarget( m_shadowMap, _T( "ShadowMap" ) );
	if ( m_rtvShadowMap == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		return false;
	}

	m_srvShadowMap = resourceMgr.CreateTextureShaderResource( m_shadowMap, _T( "ShadowMap" ) );
	if ( m_srvShadowMap == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		return false;
	}

	m_shadowDepth = resourceMgr.CreateTexture2D( _T( "ShadowMapDepthStencil" ), _T( "ShadowMapDepthStencil" ) );
	if ( m_shadowDepth == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		return false;
	}

	TEXTURE_TRAIT texTrait = resourceMgr.GetTextureTrait( m_shadowDepth );
	texTrait.m_format = RESOURCE_FORMAT::D24_UNORM_S8_UINT;

	m_dsvShadowMap = resourceMgr.CreateDepthStencil( m_shadowDepth, _T( "ShadowMapDepthStencil" ), &texTrait );
	if ( m_dsvShadowMap == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		return false;
	}

	m_shadowMapMtl = renderer.SearchMaterial( SHADOWMAP_MATERIAL_NAME );
	if ( m_shadowMapMtl == INVALID_MATERIAL )
	{
		return false;
	}

	return true;
}
