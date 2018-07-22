#include "stdafx.h"
#include "Scene/ShadowManager.h"

#include "Core/GameLogic.h"
#include "GameObject/LightManager.h"
#include "Math/CXMFloat.h"
#include "Physics/BoundingCone.h"
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

void CShadowManager::BuildShadowProjectionMatrix( CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects )
{
	gameLogic.GetUIManager( ).Window( "Shadow" );

	if ( gameLogic.GetUIManager( ).Button( "Enable / Disable" ) )
	{
		m_isEnabled = !m_isEnabled;
	}

	if ( gameLogic.GetUIManager( ).Button( "ClipUnitCube On/Off" ) )
	{
		m_isUnitClipCube = !m_isUnitClipCube;
	}

	gameLogic.GetUIManager( ).SliderFloat( "Z Bias", &m_constant.m_zBias, 0.f, 0.1f );
	
	if ( gameLogic.GetUIManager( ).Button( "Change Shadow Type" ) )
	{
		switch ( m_shadowType )
		{
		case ShadowType::Ortho:
			m_shadowType = ShadowType::PSM;
			break;
		case ShadowType::PSM:
			m_shadowType = ShadowType::LSPSM;
			break;
		case ShadowType::LSPSM:
			m_shadowType = ShadowType::Ortho;
			break;
		default:
			break;
		}
	}

	if ( m_isEnabled )
	{
		ClassifyShadowCasterAndReceiver( gameLogic, gameObjects );
		if ( m_shadowReceivers.size( ) == 0 )
		{
			return;
		}

		switch ( m_shadowType )
		{
		case ShadowType::Ortho:
			BuildOrthoShadowProjectionMatrix( gameLogic, gameObjects );
			break;
		case ShadowType::PSM:
			BuildPSMProjectionMatrix( gameLogic, gameObjects );
			break;
		case ShadowType::LSPSM:
			BuildLSPSMProjectionMatrix( gameLogic, gameObjects );
			break;
		default:
			break;
		}

		if ( ShadowConstant* pData = static_cast<ShadowConstant*>( gameLogic.GetRenderer( ).LockBuffer( m_shadowConstant ) ) )
		{
			*pData = m_constant;
			gameLogic.GetRenderer( ).UnLockBuffer( m_shadowConstant );
			gameLogic.GetRenderer( ).BindConstantBuffer( SHADER_TYPE::PS, static_cast<int>( PS_CONSTANT_BUFFER::SHADOW ), 1, &m_shadowConstant );
		}
	}
}

void CShadowManager::DrawShadowMap( CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects )
{
	if ( m_isEnabled && ( m_shadowReceivers.size( ) != 0 ) )
	{
		IRenderer& renderer = gameLogic.GetRenderer( );

		// 그림자 텍스쳐로 랜더 타겟 변경
		RE_HANDLE defaultResource[] = { RE_HANDLE_TYPE::INVALID_HANDLE };
		renderer.BindShaderResource( SHADER_TYPE::PS, 2, 1, defaultResource );
		renderer.BindRenderTargets( &m_rtvShadowMap, 1, m_dsvShadowMap );

		constexpr float clearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

		renderer.ClearRendertarget( m_rtvShadowMap, clearColor );
		renderer.ClearDepthStencil( m_dsvShadowMap, 1.0f, 0 );

		const TEXTURE_TRAIT& trait = renderer.GetResourceManager( ).GetTextureTrait( m_shadowMap );

		// 뷰포트 세팅
		CRenderView& view = gameLogic.GetView( );
		Viewport viewport = { 0, 0, static_cast<float>( trait.m_width ), static_cast<float>( trait.m_height ), 0.f, 1.f };
		view.SetViewPort( renderer, &viewport, 1 );
		RECT rect = { 0, 0, static_cast<long>( trait.m_width ), static_cast<long>( trait.m_height ) };
		view.SetScissorRects( renderer, &rect, 1 );

		// 그림자 렌더링 마테리얼로 Caster를 랜더링
		for ( auto& object : m_shadowCasters )
		{
			if ( object && object->ShouldDrawShadow( ) )
			{
				object->SetOverrideMaterial( m_shadowMapMtl );
				object->UpdateTransform( gameLogic );
				object->Render( gameLogic );
				object->SetOverrideMaterial( INVALID_MATERIAL );
			}
		}

		// 랜터 타겟 원상 복귀, 그림자 맵 세팅
		RE_HANDLE default[] = { RE_HANDLE_TYPE::INVALID_HANDLE };
		renderer.BindRenderTargets( default, 1, default[0] );
		renderer.BindShaderResource( SHADER_TYPE::PS, 2, 1, &m_srvShadowMap );
	}
}

bool CShadowManager::CreateDeviceDependentResource( IRenderer& renderer )
{
	// 그림자용 렌더 타겟 생성
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

	BUFFER_TRAIT constantTrait = {
		sizeof( ShadowConstant ),
		1,
		RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE,
		RESOURCE_TYPE::CONSTANT_BUFFER,
		0U,
		nullptr,
		0U,
		0U
	};

	m_shadowConstant = resourceMgr.CreateBuffer( constantTrait );
	if ( m_shadowConstant == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		return false;
	}

	m_shadowSampler = resourceMgr.CreateSamplerState( _T( "shadow" ) );
	if ( m_shadowSampler == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		return false;
	}

	renderer.BindSamplerState( SHADER_TYPE::PS, 1, 1, &m_shadowSampler );

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

	m_shadowCasters.clear( );
	m_shadowCasterPoints.clear( );
	m_shadowReceivers.clear( );
	m_shadowReceiverPoints.clear( );

	m_shadowCasters.reserve( count );
	m_shadowCasterPoints.reserve( count );
	m_shadowReceivers.reserve( count );
	m_shadowReceiverPoints.reserve( count );

	const CXMFLOAT4X4& viewMat = view.GetViewMatrix( );
	CXMFLOAT4X4 viewProjectionMat = XMMatrixMultiply( viewMat, view.GetProjectionMatrix( ) );
	CFrustum frustum( viewProjectionMat );

	CAaboundingbox box;

	CXMFLOAT3 sweepDir = XMVector3Normalize( primaryLight->GetDirection( ) );

	bool hit = false;

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
					hit = true;
					TransformAABB( box, *aabb, viewMat );
					m_shadowCasterPoints.emplace_back( box );
					m_shadowCasters.emplace_back( object.get( ) );
				}
			}
			break;
		case COLLISION::INSIDE:
			{
				hit = true;
				TransformAABB( box, *aabb, viewMat );
				m_shadowCasterPoints.emplace_back( box );
				m_shadowReceiverPoints.emplace_back( box );
				m_shadowCasters.emplace_back( object.get( ) );
				m_shadowReceivers.emplace_back( object.get( ) );
			}
			break;
		case COLLISION::INTERSECTION:
			{
				hit = true;
				m_shadowCasters.emplace_back( object.get( ) );
				m_shadowReceivers.emplace_back( object.get( ) );

				const std::vector<std::unique_ptr<IRigidBody>>& subBoxes = object->GetSubRigidBody( RIGID_BODY_TYPE::AABB );

				for ( const auto& subBox : subBoxes )
				{
					const CAaboundingbox* subAabb = reinterpret_cast<const CAaboundingbox*>( subBox.get() );

					if ( subAabb->Intersect( frustum ) != COLLISION::OUTSIDE )
					{
						TransformAABB( box, *subAabb, viewMat );
						m_shadowCasterPoints.emplace_back( box );
						m_shadowReceiverPoints.emplace_back( box );
					}
				}
			}
			break;
		}
	}

	m_cosGamma = -( XMVectorGetX( XMVector3Dot( sweepDir, CXMFLOAT3( viewMat._13, viewMat._23, viewMat._33 ) ) ) );

	if ( hit )
	{
		float minZ = FLT_MAX;
		float maxZ = -FLT_MAX;

		for ( size_t i = 0; i < m_shadowReceiverPoints.size( ); ++i )
		{
			minZ = min( minZ, m_shadowReceiverPoints[i].GetMin( ).z );
			maxZ = max( maxZ, m_shadowReceiverPoints[i].GetMax( ).z );
		}

		m_zNear = max( view.GetNear( ), minZ );
		m_zFar = min( view.GetFar( ), maxZ );
		m_slideBack = 0.f;
	}
	else
	{
		m_zNear = view.GetNear();
		m_zFar = view.GetFar( );
		m_slideBack = 0.f;
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
		frustumAABB = CAaboundingbox( m_shadowReceiverPoints );
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

	CAaboundingbox casterAABB( m_shadowCasterPoints );

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
		lightPosition -= viewLightDir * distance * 2.f;
	}
	else
	{
		assert( false );
	}

	CXMFLOAT3 axis = { 0.f, 1.f, 0.f };

	if ( fabsf( XMVectorGetX( XMVector3Dot( viewLightDir, axis ) ) ) > 0.99f )
	{
		axis = { 0.f, 0.f, 1.f };
	}

	CXMFLOAT4X4 lightView = XMMatrixLookAtLH( lightPosition, center, axis );

	TransformAABB( frustumAABB, frustumAABB, lightView );
	TransformAABB( casterAABB, casterAABB, lightView );

	const CXMFLOAT3& frustumMin = frustumAABB.GetMin( );
	const CXMFLOAT3& frustumMax = frustumAABB.GetMax( );
	const CXMFLOAT3& casterMin = casterAABB.GetMin( );

	CXMFLOAT4X4 lightPorjection = XMMatrixOrthographicOffCenterLH( frustumMin.x, frustumMax.x,
														frustumMin.y, frustumMax.y,
														casterMin.z, frustumMax.z );

	m_lightViewProjection = XMMatrixMultiply( view.GetViewMatrix( ), lightView );
	m_lightViewProjection = XMMatrixMultiply( m_lightViewProjection, lightPorjection );
}

void CShadowManager::BuildPSMProjectionMatrix( CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects )
{
	ImUI& ui = gameLogic.GetUIManager( );

	ui.SliderFloat( "minInfinityZ Slider", &m_minInfinityZ, 0.f, 100.f );
	ui.SameLine( );
	ui.Text( "minInfinityZ" );

	using namespace DirectX;

	const CRenderView& view = gameLogic.GetView( );

	float infinity = m_zFar / ( m_zFar - m_zNear );
	
	const float Z_EPSILON = 0.0001f;
	if ( m_isSlideBack && ( infinity <= m_minInfinityZ ) )
	{
		m_slideBack = m_minInfinityZ * ( m_zFar - m_zNear ) - m_zFar + Z_EPSILON;
		m_zFar += m_slideBack;
		m_zNear += m_slideBack;
	}

	CXMFLOAT4X4 virtualCameraView = XMMatrixTranslation( 0.f, 0.f, m_slideBack );
	CXMFLOAT4X4 virtualCameraProjection;

	if ( m_isSlideBack )
	{
		if ( m_isUnitClipCube )
		{
			CBoundingCone cone( m_shadowReceiverPoints, virtualCameraView, CXMFLOAT3( 0.f, 0.f, 0.f ), CXMFLOAT3( 0.f, 0.f, 1.f ) );
			virtualCameraProjection = XMMatrixPerspectiveLH( 2.f * tanf( cone.GetFovX( ) ) * m_zNear, 2.f * tanf( cone.GetFovY() ) * m_zNear, m_zNear, m_zFar );

			std::string viewWidth = std::string( "viewWidth : " ) + std::to_string( 2.f * tanf( cone.GetFovX( ) ) * m_zNear );
			ui.Text( viewWidth.c_str( ) );

			std::string vidwHeight = std::string( "viewHeight : " ) + std::to_string( 2.f * tanf( cone.GetFovY( ) ) * m_zNear );
			ui.Text( vidwHeight.c_str( ) );

			std::string fovX = std::string( "fovX : " ) + std::to_string( cone.GetFovX( ) );
			ui.Text( fovX.c_str( ) );

			std::string fovY = std::string( "fovY : " ) + std::to_string( cone.GetFovY( ) );
			ui.Text( fovY.c_str( ) );

			std::string zNear = std::string( "Near : " ) + std::to_string( m_zNear );
			ui.Text( zNear.c_str( ) );

			std::string zFar = std::string( "Far : " ) + std::to_string( m_zFar );
			ui.Text( zFar.c_str( ) );
		}
		else
		{
			float viewHeight = view.GetFar( ) * tanf( 0.5f * view.GetFov( ) );
			float viewWidth = view.GetFov( ) * viewHeight;

			float halfFovy = atanf( viewHeight / ( view.GetFar() + m_slideBack ) ) ;
			float halfFovx = atanf( viewWidth / ( view.GetFar() + m_slideBack ) );

			virtualCameraProjection = XMMatrixPerspectiveLH( 2.f * tanf( halfFovx ) * m_zNear, 2.f * tanf( halfFovy ) * m_zNear, m_zNear, m_zFar );
		}
	}
	else
	{
		virtualCameraView = XMMatrixIdentity( );
		virtualCameraProjection = XMMatrixPerspectiveFovLH( view.GetFov( ), view.GetAspect( ), m_zNear, m_zFar );
	}

	CLightManager& lightMgr = gameLogic.GetLightManager( );
	CLight* primaryLight = lightMgr.GetPrimaryLight( );

	if ( primaryLight == nullptr )
	{
		__debugbreak( );
	}

	CXMFLOAT4 lightPos = XMVector3TransformNormal( -primaryLight->GetDirection( ), view.GetViewMatrix( ) );
	lightPos = XMVector3TransformNormal( lightPos, virtualCameraProjection );

	CXMFLOAT4X4 lightView;
	CXMFLOAT4X4 lightProjection;

	if ( fabsf( lightPos.w ) <= 0.001f )
	{
		CXMFLOAT3 ppLightDir( lightPos.x, lightPos.y, lightPos.z );
		ppLightDir = XMVector3Normalize( ppLightDir );

		CAaboundingbox ppUnitBox;
		ppUnitBox.SetMax( 1, 1, 1 );
		ppUnitBox.SetMin( -1, -1, 0 );

		CXMFLOAT3 cubeCenter;
		ppUnitBox.Centroid( cubeCenter );

		CXMFLOAT3 rayOrigin = ppLightDir * 2.f;
		rayOrigin += cubeCenter;

		CRay ray( rayOrigin, -ppLightDir );
		float t = ppUnitBox.Intersect( &ray );

		CXMFLOAT3 lightPosition;
		if ( t > 0.f )
		{
			CXMFLOAT3 collsionPos = rayOrigin;
			collsionPos -= ppLightDir * t;

			float distance = XMVectorGetX( XMVector3Length( cubeCenter - collsionPos ) );
			lightPosition = cubeCenter;
			lightPosition += ppLightDir * distance * 2.f;
		}
		else
		{
			assert( false );
		}

		CXMFLOAT3 axis = { 0.f, 1.f, 0.f };

		if ( fabsf( XMVectorGetX( XMVector3Dot( ppLightDir, axis ) ) ) > 0.99f )
		{
			axis = { 0.f, 0.f, 1.f };
		}

		lightView = XMMatrixLookAtLH( lightPosition, cubeCenter, axis );
		
		TransformAABB( ppUnitBox, ppUnitBox, lightView );
		const CXMFLOAT3& unitBoxMin = ppUnitBox.GetMin( );
		const CXMFLOAT3& unitBoxMax = ppUnitBox.GetMax( );

		lightProjection = XMMatrixOrthographicOffCenterLH( unitBoxMin.x, unitBoxMax.x, unitBoxMin.y, unitBoxMax.y, unitBoxMin.z, unitBoxMax.z );
	}
	else
	{
		CXMFLOAT3 ppLightPos = lightPos / lightPos.w;

		CXMFLOAT4X4 eyeToPostProjectiveVirtualCamera;
		eyeToPostProjectiveVirtualCamera = XMMatrixMultiply( virtualCameraView, virtualCameraProjection );

		bool isShadowTestInverted = ( lightPos.w < 0.f );
		if ( isShadowTestInverted )
		{
			CBoundingCone viewCone;
			if ( m_isUnitClipCube )
			{
				viewCone = CBoundingCone( m_shadowReceiverPoints, eyeToPostProjectiveVirtualCamera, ppLightPos );
			}
			else
			{
				std::vector<CAaboundingbox> oneBox;
				oneBox.emplace_back( );
				oneBox[0].SetMax( 1.f, 1.f, 1.f );
				oneBox[0].SetMin( -1.f, -1.f, 0.f );
				viewCone = CBoundingCone( oneBox, XMMatrixIdentity(), ppLightPos );
			}
			lightView = viewCone.GetLookAt( );
			
			float fNear = max( 0.001f, viewCone.GetNear( ) * 0.3f );
			lightProjection = XMMatrixPerspectiveLH( 2.f * tanf( viewCone.GetFovX( ) ) * -fNear, 2.f * tanf( viewCone.GetFovY( ) ) * -fNear, -fNear, fNear );
		}
		else
		{
			float fFovy;
			float fAspect;
			float fNear;
			float fFar;

			if ( m_isUnitClipCube )
			{
				CBoundingCone viewCone( m_shadowReceiverPoints, eyeToPostProjectiveVirtualCamera, ppLightPos );
				lightView = viewCone.GetLookAt( );

				fFovy = viewCone.GetFovY( ) * 2.f;
				fAspect = viewCone.GetFovX( ) / viewCone.GetFovY( );
				fFar = viewCone.GetFar( );
				fNear = viewCone.GetNear( );
			}
			else
			{
				CXMFLOAT3 lookAt = CXMFLOAT3( 0.f, 0.f, 0.5f ) - ppLightPos;

				float distance = XMVectorGetX( XMVector3Length( lookAt ) );
				lookAt /= distance;

				CXMFLOAT3 axis = { 0.f, 1.f, 0.f };

				if ( fabsf( XMVectorGetX( XMVector3Dot( lookAt, axis ) ) ) > 0.99f )
				{
					axis = { 0.f, 0.f, 1.f };
				}

				lightView = XMMatrixLookAtLH( ppLightPos, CXMFLOAT3( 0.f, 0.f, 0.5f ), axis );

				constexpr float ppCubeRadius = 1.5f;  // the post-projective view box radius is 1.5
				fFovy = 2.f * atanf( ppCubeRadius / distance );
				fAspect = 1.f;
				fNear = distance - 2.f * ppCubeRadius;
				fFar = distance + 2.f * ppCubeRadius;
			}

			fNear = max( 0.001f, fNear );
			lightProjection = XMMatrixPerspectiveFovLH( fFovy, fAspect, fNear, fFar );
		}
	}

	m_lightViewProjection = XMMatrixMultiply( view.GetViewMatrix( ), virtualCameraView );
	m_lightViewProjection = XMMatrixMultiply( m_lightViewProjection, virtualCameraProjection );
	m_lightViewProjection = XMMatrixMultiply( m_lightViewProjection, lightView );
	m_lightViewProjection = XMMatrixMultiply( m_lightViewProjection, lightProjection );
}

void CShadowManager::BuildLSPSMProjectionMatrix( CGameLogic& gameLogic, std::vector<std::unique_ptr<CGameObject>>& gameObjects )
{
	if ( abs( m_cosGamma ) >= 0.999f )
	{
		BuildOrthoShadowProjectionMatrix( gameLogic, gameObjects );
	}
	else
	{
		std::vector<CXMFLOAT3> bodyB;
		bodyB.reserve( m_shadowCasterPoints.size( ) * 8 + 8 );

		const CRenderView& view = gameLogic.GetView( );

		CFrustum eyeFrustum( view.GetProjectionMatrix( ) );
		const CXMFLOAT3 ( &frustumVertices )[8] = eyeFrustum.GetVertices( );
		for ( int i = 0; i < 8; ++i )
		{
			bodyB.emplace_back( frustumVertices[i] );
		}

		for ( const auto& box : m_shadowCasterPoints )
		{
			for ( int i = 0; i < 8; ++i )
			{
				bodyB.emplace_back( box.Point( i ) );
			}
		}

		CLightManager& lightMgr = gameLogic.GetLightManager( );
		CLight* primaryLight = lightMgr.GetPrimaryLight( );

		if ( primaryLight == nullptr )
		{
			__debugbreak( );
		}

		CXMFLOAT3 upVector = XMVector3Normalize( XMVector3TransformNormal( -primaryLight->GetDirection( ), view.GetViewMatrix() ) );

		const CXMFLOAT3 eyeVector( 0.f, 0.f, 1.f );
		CXMFLOAT3 rightVector = XMVector3Normalize( XMVector3Cross( upVector, eyeVector ) );
		CXMFLOAT3 lookVector = XMVector3Normalize( XMVector3Cross( rightVector, upVector ) );

		CXMFLOAT4X4 lightSpaceBasis(
			rightVector.x, upVector.x, lookVector.x, 0.f,
			rightVector.y, upVector.y, lookVector.y, 0.f,
			rightVector.z, upVector.z, lookVector.z, 0.f,
			0.f, 0.f, 0.f, 1.f
		);

		XMVector3TransformCoordStream( bodyB.data( ), sizeof( CXMFLOAT3 ), bodyB.data( ), sizeof( CXMFLOAT3 ), bodyB.size( ), lightSpaceBasis );

		CAaboundingbox lightSpaceBox( bodyB );
		CXMFLOAT3 lightSpaceOrigin;
		lightSpaceBox.Centroid( lightSpaceOrigin );
		float sinGamma = sqrtf( 1.f - m_cosGamma * m_cosGamma );

		float nOpt0 = m_zNear + sqrtf( m_zNear * m_zFar );

		float zNear = view.GetNear( );
		float zFar = view.GetFar( );
		float nOpt1 = zNear + sqrtf( zNear * zFar );

		float lspsmNopt = ( ( nOpt0 + nOpt1 ) / ( 2.f * sinGamma ) ) + 0.1f;

		gameLogic.GetUIManager( ).SliderFloat( "nOpt Weight", &m_nOptWeight, 0.f, 1.f );
		float nOpt = 0.1f + m_nOptWeight * ( lspsmNopt - 0.1f );

		lightSpaceOrigin.z = lightSpaceBox.GetMin( ).z - nOpt;

		float fovX = -FLT_MAX, fovY = -FLT_MAX, maxZ = -FLT_MAX;

		CXMFLOAT3 tmp;
		for ( const auto& point : bodyB )
		{
			tmp = point - lightSpaceOrigin;

			fovX = max( fovX, abs( tmp.x / tmp.z ) );
			fovY = max( fovY, abs( tmp.y / tmp.z ) );
			maxZ = max( maxZ, tmp.z );
		}

		CXMFLOAT4X4 translate = XMMatrixTranslation( -lightSpaceOrigin.x, -lightSpaceOrigin.y, -lightSpaceOrigin.z );
		CXMFLOAT4X4 perspective = XMMatrixPerspectiveLH( 2.f * fovX * nOpt, 2.f * fovY * nOpt, nOpt, maxZ );

		lightSpaceBasis = XMMatrixMultiply( lightSpaceBasis, translate );
		lightSpaceBasis = XMMatrixMultiply( lightSpaceBasis, perspective );

		CXMFLOAT4X4 permute(
			1.f, 0.f, 0.f, 0.f,
			0.f, 0.f, -1.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, -0.5f, 1.5f, 1.f
		);

		CXMFLOAT4X4 ortho = XMMatrixOrthographicLH( 2.f, 1.f, 0.5f, 2.5f );

		lightSpaceBasis = XMMatrixMultiply( lightSpaceBasis, permute );
		lightSpaceBasis = XMMatrixMultiply( lightSpaceBasis, ortho );

		if ( m_isUnitClipCube )
		{
			std::vector<CXMFLOAT3> receiverPoints;
			receiverPoints.reserve( m_shadowReceiverPoints.size( ) * 8 );

			for ( const auto& box : m_shadowReceiverPoints )
			{
				for ( int i = 0; i < 8; ++i )
				{
					receiverPoints.emplace_back( box.Point( i ) );
				}
			}

			XMVector3TransformCoordStream( receiverPoints.data( ), sizeof( CXMFLOAT3 ), receiverPoints.data( ), sizeof( CXMFLOAT3 ), receiverPoints.size( ), lightSpaceBasis );

			CAaboundingbox receiverBox( receiverPoints );
			
			float minX = min( -1.f, receiverBox.GetMin().x );
			float minY = min( -1.f, receiverBox.GetMin( ).y );
			float maxX = max( 1.f, receiverBox.GetMax( ).x );
			float maxY = max( 1.f, receiverBox.GetMax( ).y );

			float boxWidth = maxX - minX;
			float boxHeight = maxY - minY;

			if ( fpclassify( boxWidth ) != FP_ZERO && fpclassify( boxHeight ) != FP_ZERO )
			{
				float boxX = ( minX + maxX ) * 0.5f;
				float boxY = ( minY + maxY ) * 0.5f;

				CXMFLOAT4X4 clip(
					2.f / boxWidth, 0.f, 0.f, 0.f,
					0.f, 2.f / boxHeight, 0.f, 0.f,
					0.f, 0.f, 1.f, 0.f,
					-2.f * boxX / boxWidth, -2.f * boxY / boxHeight, 0.f, 1.f
				);

				lightSpaceBasis = XMMatrixMultiply( lightSpaceBasis, clip );
			}
		}

		m_lightViewProjection = XMMatrixMultiply( view.GetViewMatrix( ), lightSpaceBasis );
	}
}
