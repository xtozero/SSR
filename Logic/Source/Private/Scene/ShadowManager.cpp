#include "stdafx.h"
#include "Scene/ShadowManager.h"

#include "Core/GameLogic.h"
#include "GameObject/LightManager.h"
#include "Math/CXMFloat.h"
#include "Physics/BoundingSphere.h"
#include "Physics/Frustum.h"
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

void CShadowManager::Process( CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects )
{
	if ( m_isEnabled )
	{
		ClassifyShadowCasterAndReceiver( gameLogic, gameObjects );
		if ( m_shadowReceiver.size( ) == 0 )
		{
			return;
		}

		BuildOrthoShadowProjectionMatrix( gameLogic, gameObjects );

		using namespace SHARED_CONSTANT_BUFFER;
		RE_HANDLE shadowBuffer = gameLogic.GetCommonConstantBuffer( VS_SHADOW );

		//그림자 렌더링에 사용할 조명으로 View 행렬을 만들어 세팅
		IRenderer& renderer = gameLogic.GetRenderer( );
		ShadowTransform* buffer = static_cast<ShadowTransform*>( renderer.LockBuffer( shadowBuffer ) );

		if ( buffer )
		{
			buffer->m_lightView = XMMatrixTranspose( m_lightView );
			buffer->m_lightProjection = XMMatrixTranspose( m_lightPorjection );

			renderer.UnLockBuffer( shadowBuffer );
			renderer.BindConstantBuffer( SHADER_TYPE::VS, VS_CONSTANT_BUFFER::SHADOW, 1, &shadowBuffer );
		}
		else
		{
			__debugbreak( );
		}

		//그림자 텍스쳐로 랜더 타겟 변경
		RE_HANDLE defaultResource[] = { RE_HANDLE_TYPE::INVALID_HANDLE };
		renderer.BindShaderResource( SHADER_TYPE::PS, 2, 1, defaultResource );
		renderer.BindRenderTargets( &m_rtvShadowMap, 1, m_dsvShadowMap );

		constexpr float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		renderer.ClearRendertarget( m_rtvShadowMap, clearColor );
		renderer.ClearDepthStencil( m_dsvShadowMap, 1.0f, 0 );

		const TEXTURE_TRAIT& trait = renderer.GetResourceManager( ).GetTextureTrait( m_shadowMap );

		//뷰포트 세팅
		CRenderView& view = gameLogic.GetView( );
		view.PopViewPort( );
		view.PushViewPort( 0, 0, static_cast<float>( trait.m_width ), static_cast<float>( trait.m_height ) );
		view.SetViewPort( renderer );
		view.SetScissorRects( renderer );
		view.UpdataView( renderer, gameLogic.GetCommonConstantBuffer( VS_VIEW_PROJECTION ) );

		//그림자 렌더링 마테리얼로 Caster를 랜더링
		for ( auto& object : m_shadowCaster )
		{
			if ( object && object->ShouldDrawShadow( ) )
			{
				object->SetOverrideMaterial( m_shadowMapMtl );
				object->UpdateTransform( gameLogic );
				object->Render( gameLogic );
				object->SetOverrideMaterial( INVALID_MATERIAL );
			}
		}

		//뷰포트 원상 복귀
		view.PopViewPort( );
		view.PopScissorRect( );

		//랜터 타겟 원상 복귀, 그림자 맵 세팅
		RE_HANDLE default[] = { RE_HANDLE_TYPE::INVALID_HANDLE };
		renderer.BindRenderTargets( default, 1, default[0] );
		renderer.BindShaderResource( SHADER_TYPE::PS, 2, 1, &m_srvShadowMap );
	}
}

bool CShadowManager::CreateDeviceDependentResource( IRenderer& renderer )
{
	//그림자용 렌더 타겟 생성
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

void CShadowManager::ClassifyShadowCasterAndReceiver( CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects )
{
	using namespace DirectX;

	CLightManager& lightMgr = gameLogic.GetLightManager( );
	const CRenderView& view = gameLogic.GetView( );

	CLight* primaryLight = lightMgr.GetPrimaryLight( );

	if ( primaryLight == nullptr )
	{
		__debugbreak( );
	}

	size_t count = gameObjects.size();

	m_shadowCaster.clear( );
	m_shadowCasterPoint.clear( );
	m_shadowReceiver.clear( );
	m_shadowReceiverPoint.clear( );

	m_shadowCaster.reserve( count );
	m_shadowCasterPoint.reserve( count );
	m_shadowReceiver.reserve( count );
	m_shadowReceiverPoint.reserve( count );

	const CXMFLOAT4X4& viewMat = view.GetViewMatrix( );
	CXMFLOAT4X4 viewProjectionMat = XMMatrixMultiply( viewMat, view.GetProjectionMatrix( ) );
	CFrustum frustum( viewProjectionMat );

	CAaboundingbox box;

	CXMFLOAT3 sweepDir = XMVector3Normalize( primaryLight->GetDirection( ) );

	for ( auto& object : gameObjects )
	{
		const CAaboundingbox* aabb = reinterpret_cast<const CAaboundingbox*>( object->GetRigidBody( RIGID_BODY_TYPE::AABB ) );
		if ( aabb == nullptr )
		{
			continue;
		}

		int inFrustum = aabb->Intersect( frustum );

		switch ( inFrustum )
		{
		case COLLISION::OUTSIDE:
			{
				BoundingSphere sphere( *aabb );
				if ( sphere.Intersect( frustum, sweepDir ) )
				{
					TransformAABB( box, *aabb, viewMat );
					m_shadowCasterPoint.emplace_back( box );
					m_shadowCaster.emplace_back( object.get( ) );
				}
			}
			break;
		case COLLISION::INSIDE:
			{
				TransformAABB( box, *aabb, viewMat );
				m_shadowCasterPoint.emplace_back( box );
				m_shadowReceiverPoint.emplace_back( box );
				m_shadowCaster.emplace_back( object.get( ) );
				m_shadowReceiver.emplace_back( object.get( ) );
			}
			break;
		case COLLISION::INTERSECTION:
			{
				TransformAABB( box, *aabb, viewMat );
				m_shadowCasterPoint.emplace_back( box );
				m_shadowReceiverPoint.emplace_back( box );
				m_shadowCaster.emplace_back( object.get( ) );
				m_shadowReceiver.emplace_back( object.get( ) );
			}
			break;
		}
	}
}

void CShadowManager::BuildOrthoShadowProjectionMatrix( CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects )
{
	using namespace DirectX;

	CLightManager& lightMgr = gameLogic.GetLightManager( );
	const CRenderView& view = gameLogic.GetView( );

	CLight* primaryLight = lightMgr.GetPrimaryLight( );

	if ( primaryLight == nullptr )
	{
		__debugbreak( );
	}

	CXMFLOAT3 viewLightDir = XMVector3TransformNormal( primaryLight->GetDirection(), view.GetViewMatrix() );
	viewLightDir = XMVector3Normalize( viewLightDir );

	CAaboundingbox frustumAABB;
	if ( m_isUnitClipCube )
	{
		frustumAABB = CAaboundingbox( m_shadowReceiverPoint );
	}
	else
	{
		float height = view.GetFov( );
		float width = height * view.GetAspect( );
		float zNear = view.GetNear( );
		float zFar = view.GetFar( );

		frustumAABB.SetMax( width * zFar, height * zFar, zFar );
		frustumAABB.SetMin( -width * zFar, -height * zFar, zNear );
	}

	CAaboundingbox casterAABB( m_shadowCasterPoint );

	CXMFLOAT3 center;
	frustumAABB.Centroid( center );

	CXMFLOAT3 rayOrigin = -viewLightDir * view.GetFar( );
	rayOrigin += center;

	CRay ray( rayOrigin, viewLightDir );
	float t = casterAABB.Intersect( &ray );

	CXMFLOAT3 lightPosition = primaryLight->GetPosition( );
	if ( t > 0.f )
	{
		CXMFLOAT3 collsionPos = rayOrigin;
		collsionPos += viewLightDir * t;

		float distance = XMVectorGetX( XMVector3Length( center - collsionPos ) );
		lightPosition = center;
		lightPosition -= viewLightDir * distance * 2;
	}
	else
	{
		assert( false );
	}

	CXMFLOAT3 axis = { 0.f, 1.f, 0.f };

	if ( fabsf( XMVectorGetX( XMVector3Dot( -viewLightDir, g_XMIdentityR1 ) ) ) > 0.99f )
	{
		axis = { 0.f, 0.f, 1.f };
	}

	m_lightView = XMMatrixLookAtLH( lightPosition, center, axis );

	TransformAABB( frustumAABB, frustumAABB, m_lightView );
	TransformAABB( casterAABB, casterAABB, m_lightView );

	const CXMFLOAT3& frustumMin = frustumAABB.GetMin( );
	const CXMFLOAT3& frustumMax = frustumAABB.GetMax( );
	const CXMFLOAT3& casterMin = casterAABB.GetMin( );

	m_lightView = XMMatrixMultiply( view.GetViewMatrix( ), m_lightView );
	m_lightPorjection = XMMatrixOrthographicOffCenterLH( frustumMin.x, frustumMax.x,
														frustumMin.y, frustumMax.y,
														casterMin.z, frustumMax.z );
}
