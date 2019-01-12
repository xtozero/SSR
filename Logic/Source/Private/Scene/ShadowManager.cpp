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
	inline XMMATRIX XM_CALLCONV XMMatrixPerspectiveLH_NoAssert
	(
		float ViewWidth,
		float ViewHeight,
		float NearZ,
		float FarZ
	)
	{
#if defined(_XM_NO_INTRINSICS_)

		float TwoNearZ = NearZ + NearZ;
		float fRange = FarZ / ( FarZ - NearZ );

		XMMATRIX M;
		M.m[0][0] = TwoNearZ / ViewWidth;
		M.m[0][1] = 0.0f;
		M.m[0][2] = 0.0f;
		M.m[0][3] = 0.0f;

		M.m[1][0] = 0.0f;
		M.m[1][1] = TwoNearZ / ViewHeight;
		M.m[1][2] = 0.0f;
		M.m[1][3] = 0.0f;

		M.m[2][0] = 0.0f;
		M.m[2][1] = 0.0f;
		M.m[2][2] = fRange;
		M.m[2][3] = 1.0f;

		M.m[3][0] = 0.0f;
		M.m[3][1] = 0.0f;
		M.m[3][2] = -fRange * NearZ;
		M.m[3][3] = 0.0f;
		return M;

#elif defined(_XM_ARM_NEON_INTRINSICS_)
		float TwoNearZ = NearZ + NearZ;
		float fRange = FarZ / ( FarZ - NearZ );
		const XMVECTOR Zero = vdupq_n_f32( 0 );
		XMMATRIX M;
		M.r[0] = vsetq_lane_f32( TwoNearZ / ViewWidth, Zero, 0 );
		M.r[1] = vsetq_lane_f32( TwoNearZ / ViewHeight, Zero, 1 );
		M.r[2] = vsetq_lane_f32( fRange, g_XMIdentityR3.v, 2 );
		M.r[3] = vsetq_lane_f32( -fRange * NearZ, Zero, 2 );
		return M;
#elif defined(_XM_SSE_INTRINSICS_)
		XMMATRIX M;
		float TwoNearZ = NearZ + NearZ;
		float fRange = FarZ / ( FarZ - NearZ );
		// Note: This is recorded on the stack
		XMVECTOR rMem = {
			TwoNearZ / ViewWidth,
			TwoNearZ / ViewHeight,
			fRange,
			-fRange * NearZ
		};
		// Copy from memory to SSE register
		XMVECTOR vValues = rMem;
		XMVECTOR vTemp = _mm_setzero_ps( );
		// Copy x only
		vTemp = _mm_move_ss( vTemp, vValues );
		// TwoNearZ / ViewWidth,0,0,0
		M.r[0] = vTemp;
		// 0,TwoNearZ / ViewHeight,0,0
		vTemp = vValues;
		vTemp = _mm_and_ps( vTemp, g_XMMaskY );
		M.r[1] = vTemp;
		// x=fRange,y=-fRange * NearZ,0,1.0f
		vValues = _mm_shuffle_ps( vValues, g_XMIdentityR3, _MM_SHUFFLE( 3, 2, 3, 2 ) );
		// 0,0,fRange,1.0f
		vTemp = _mm_setzero_ps( );
		vTemp = _mm_shuffle_ps( vTemp, vValues, _MM_SHUFFLE( 3, 0, 0, 0 ) );
		M.r[2] = vTemp;
		// 0,0,-fRange * NearZ,0
		vTemp = _mm_shuffle_ps( vTemp, vValues, _MM_SHUFFLE( 2, 1, 0, 0 ) );
		M.r[3] = vTemp;
		return M;
#endif
	}
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
	ImUI& ui = gameLogic.GetUIManager( );
	ui.Window( "Shadow" );

	const char* shadowTypeText[] = { "Ortho", "PSM", "LSPSM" };
	ui.Combo( "Shadow Type", reinterpret_cast<int*>( &m_shadowType ), shadowTypeText, _countof( shadowTypeText ) );

	if ( ui.Button( "Cascaded ShadowMap On/Off" ) )
	{
		IRenderer& renderer = gameLogic.GetRenderer( );
		IResourceManager& resourceMgr = renderer.GetResourceManager( );
		DestroyShadowmapTexture( resourceMgr );
		if ( m_useCascaded )
		{
			CreateNonCascadedResource( resourceMgr );

			m_shadowMapMtl = renderer.SearchMaterial( _T( "mat_shadowMap" ) );
		}
		else
		{
			CreateCascadedResource( resourceMgr );

			m_shadowMapMtl = renderer.SearchMaterial( _T( "mat_cascaded_shadowMap" ) );
		}

		m_useCascaded = !m_useCascaded;
	}

	if ( ui.Button( "Enable / Disable" ) )
	{
		m_isEnabled = !m_isEnabled;
	}

	if ( ui.Button( "ClipUnitCube On/Off" ) )
	{
		m_isUnitClipCube = !m_isUnitClipCube;
	}

	ui.SliderFloat( "Z Bias", &m_zBias, 0.f, 10.f );
	ui.SameLine( );
	ui.Text( "Z Bias" );

	if ( m_isEnabled )
	{
		ClassifyShadowCasterAndReceiver( gameLogic, gameObjects );
		if ( m_shadowReceivers.size( ) == 0 )
		{
			return;
		}

		float zClipNear[MAX_CASCADED_NUM] = { -FLT_MAX, -FLT_MAX };
		float zClipFar[MAX_CASCADED_NUM] = { FLT_MAX, FLT_MAX };

		if ( m_useCascaded )
		{
			float interval = ( m_receiversFar - m_castersNear ) / MAX_CASCADED_NUM;

			for ( int i = 0; i < MAX_CASCADED_NUM; ++i )
			{
				zClipNear[i] = ( i == 0 ) ? m_castersNear : zClipFar[i - 1];
				zClipFar[i] = ( i == ( MAX_CASCADED_NUM - 1 ) ) ? m_receiversFar : zClipNear[i] + interval;
				m_cascadeConstant[i].m_zFar = zClipFar[i];
			}
		}
		else
		{
			m_cascadeConstant[0].m_zFar = FLT_MAX;
		}

		for ( int i = 0, end = m_useCascaded ? MAX_CASCADED_NUM : 1; i < end; ++i )
		{
			switch ( m_shadowType )
			{
			case ShadowType::ORTHO:
				BuildOrthoShadowProjectionMatrix( gameLogic, i, zClipNear[i], zClipFar[i] );
				break;
			case ShadowType::PSM:
				BuildPSMProjectionMatrix( gameLogic, i, zClipNear[i], zClipFar[i] );
				break;
			case ShadowType::LSPSM:
				BuildLSPSMProjectionMatrix( gameLogic, i, zClipNear[i], zClipFar[i] );
				break;
			default:
				break;
			}
		}

		if ( PsCascadeConstant* pData = static_cast<PsCascadeConstant*>( gameLogic.GetRenderer( ).LockBuffer( m_psShadowConstant ) ) )
		{
			for ( int i = 0; i < MAX_CASCADED_NUM; ++i )
			{
				pData->m_cascadeConstant[i].m_zFar = m_cascadeConstant[i].m_zFar;
				pData->m_lightViewProjection[i] = XMMatrixMultiplyTranspose( m_lightView[i], m_lightProjection[i] );
			}

			gameLogic.GetRenderer( ).UnLockBuffer( m_psShadowConstant );
			gameLogic.GetRenderer( ).BindConstantBuffer( SHADER_TYPE::PS, static_cast<int>( PS_CONSTANT_BUFFER::SHADOW ), 1, &m_psShadowConstant );
		}

		if ( m_useCascaded )
		{
			if ( GsCascadeConstant* pData = static_cast<GsCascadeConstant*>( gameLogic.GetRenderer( ).LockBuffer( m_gsShadowConstant ) ) )
			{
				pData->m_zBias = m_zBias;
				for ( int i = 0; i < MAX_CASCADED_NUM; ++i )
				{
					pData->m_lightView[i] = XMMatrixTranspose( m_lightView[i] );
					pData->m_lightProjection[i] = XMMatrixTranspose( m_lightProjection[i] );
				}
				gameLogic.GetRenderer( ).UnLockBuffer( m_gsShadowConstant );
				gameLogic.GetRenderer( ).BindConstantBuffer( SHADER_TYPE::GS, static_cast<int>( GS_CONSTANT_BUFFER::SHADOW ), 1, &m_gsShadowConstant );
			}
		}
		else
		{
			if ( VsNonCascadeConstant* pData = static_cast<VsNonCascadeConstant*>( gameLogic.GetRenderer( ).LockBuffer( m_vsShadowConstant ) ) )
			{
				pData->m_zBias = m_zBias;
				pData->m_lightView = XMMatrixTranspose( m_lightView[0] );
				pData->m_lightProjection = XMMatrixTranspose( m_lightProjection[0] );
				gameLogic.GetRenderer( ).UnLockBuffer( m_vsShadowConstant );
				gameLogic.GetRenderer( ).BindConstantBuffer( SHADER_TYPE::VS, static_cast<int>( VS_CONSTANT_BUFFER::SHADOW ), 1, &m_vsShadowConstant );
			}
		}
	}

	ui.EndWindow( );
}

void CShadowManager::DrawShadowMap( CGameLogic& gameLogic )
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
	if ( m_useCascaded )
	{
		if ( CreateCascadedResource( resourceMgr ) == false )
		{
			return false;
		}

		m_shadowMapMtl = renderer.SearchMaterial( _T( "mat_cascaded_shadowMap" ) );
	}
	else
	{
		if ( CreateNonCascadedResource( resourceMgr ) == false )
		{
			return false;
		}

		m_shadowMapMtl = renderer.SearchMaterial( _T( "mat_shadowMap" ) );
	}

	if ( m_shadowMapMtl == INVALID_MATERIAL )
	{
		return false;
	}

	BUFFER_TRAIT constantTrait = {
		sizeof( PsCascadeConstant ),
		1,
		RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE,
		RESOURCE_TYPE::CONSTANT_BUFFER,
		0U,
		nullptr,
		0U,
		0U
	};

	m_psShadowConstant = resourceMgr.CreateBuffer( constantTrait );
	if ( m_psShadowConstant == RE_HANDLE_TYPE::INVALID_HANDLE )
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

bool CShadowManager::CreateNonCascadedResource( IResourceManager& resourceMgr )
{
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

	BUFFER_TRAIT constantTrait = {
		sizeof( VsNonCascadeConstant ),
		1,
		RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE,
		RESOURCE_TYPE::CONSTANT_BUFFER,
		0U,
		nullptr,
		0U,
		0U
	};

	m_vsShadowConstant = resourceMgr.CreateBuffer( constantTrait );
	if ( m_vsShadowConstant == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		return false;
	}

	return true;
}

bool CShadowManager::CreateCascadedResource( IResourceManager& resourceMgr )
{
	m_shadowMap = resourceMgr.CreateTexture2D( _T( "CascadedShadowMap" ), _T( "CascadedShadowMap" ) );
	if ( m_shadowMap == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		return false;
	}

	m_rtvShadowMap = resourceMgr.CreateRenderTarget( m_shadowMap, _T( "CascadedShadowMap" ) );
	if ( m_rtvShadowMap == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		return false;
	}

	m_srvShadowMap = resourceMgr.CreateTextureShaderResource( m_shadowMap, _T( "CascadedShadowMap" ) );
	if ( m_srvShadowMap == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		return false;
	}

	m_shadowDepth = resourceMgr.CreateTexture2D( _T( "CascadedShadowMapDepthStencil" ), _T( "CascadedShadowMapDepthStencil" ) );
	if ( m_shadowDepth == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		return false;
	}

	TEXTURE_TRAIT texTrait = resourceMgr.GetTextureTrait( m_shadowDepth );
	texTrait.m_format = RESOURCE_FORMAT::D24_UNORM_S8_UINT;

	m_dsvShadowMap = resourceMgr.CreateDepthStencil( m_shadowDepth, _T( "CascadedShadowMapDepthStencil" ), &texTrait );
	if ( m_dsvShadowMap == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		return false;
	}

	BUFFER_TRAIT constantTrait = {
		sizeof( GsCascadeConstant ),
		1,
		RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE,
		RESOURCE_TYPE::CONSTANT_BUFFER,
		0U,
		nullptr,
		0U,
		0U
	};

	m_gsShadowConstant = resourceMgr.CreateBuffer( constantTrait );
	if ( m_gsShadowConstant == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		return false;
	}

	return true;
}

void CShadowManager::DestroyShadowmapTexture( IResourceManager& resourceMgr )
{
	resourceMgr.FreeResource( m_srvShadowMap );
	resourceMgr.FreeResource( m_rtvShadowMap );
	resourceMgr.FreeResource( m_shadowMap );

	resourceMgr.FreeResource( m_dsvShadowMap );
	resourceMgr.FreeResource( m_shadowDepth );
}

void CShadowManager::DestroyNonCascadeResource( IResourceManager& resourceMgr )
{
	DestroyShadowmapTexture( resourceMgr );

	resourceMgr.FreeResource( m_vsShadowConstant );
}

void CShadowManager::DestoryCascadedResource( IResourceManager& resourceMgr )
{
	DestroyShadowmapTexture( resourceMgr );

	resourceMgr.FreeResource( m_gsShadowConstant );
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
		const CAaboundingbox* aabb = reinterpret_cast<const CAaboundingbox*>( object->GetCollider( COLLIDER::AABB ) );
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

				const std::vector<std::unique_ptr<ICollider>>& subBoxes = object->GetSubColliders( COLLIDER::AABB );

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

	m_cosGamma = XMVectorGetX( XMVector3Dot( -sweepDir, CXMFLOAT3( viewMat._13, viewMat._23, viewMat._33 ) ) );

	if ( hit )
	{
		float minZ = FLT_MAX;
		float maxZ = -FLT_MAX;

		for ( const CAaboundingbox& aabb : m_shadowReceiverPoints )
		{
			minZ = min( minZ, aabb.GetMin( ).z );
			maxZ = max( maxZ, aabb.GetMax( ).z );
		}

		m_receiversNear = max( view.GetNear( ), minZ );
		m_receiversFar = min( view.GetFar( ), maxZ );
		m_slideBack = 0.f;
	}
	else
	{
		m_receiversNear = view.GetNear();
		m_receiversFar = view.GetFar( );
		m_slideBack = 0.f;
	}

	m_castersNear = FLT_MAX;
	m_castersFar = -FLT_MAX;

	for ( const CAaboundingbox& aabb : m_shadowCasterPoints )
	{
		m_castersNear = min( m_castersNear, aabb.GetMin( ).z );
		m_castersFar = max( m_castersFar, aabb.GetMax( ).z );
	}
}

void CShadowManager::BuildOrthoShadowProjectionMatrix( CGameLogic& gameLogic, int cascadeLevel, float zClipNear, float zClipFar )
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
		for ( const CAaboundingbox& aabb : m_shadowReceiverPoints )
		{
			const CXMFLOAT3& max = aabb.GetMax( );
			const CXMFLOAT3& min = aabb.GetMin( );

			if ( min.z < zClipFar && max.z > zClipNear )
			{
				frustumAABB.Merge( max );
				frustumAABB.Merge( min );
			}
		}
	}
	else
	{
		float height = view.GetFov( );
		float width = height * view.GetAspect( );
		float zNear = max( view.GetNear( ), zClipNear );
		float zFar = min( view.GetFar( ), zClipFar );

		frustumAABB.SetMax( width * zFar, height * zFar, zFar );
		frustumAABB.SetMin( -width * zFar, -height * zFar, zNear );
	}

	CAaboundingbox casterAABB;
	for ( const CAaboundingbox& aabb : m_shadowCasterPoints )
	{
		const CXMFLOAT3& max = aabb.GetMax( );
		const CXMFLOAT3& min = aabb.GetMin( );

		if ( min.z < zClipFar && max.z > zClipNear )
		{
			casterAABB.Merge( max );
			casterAABB.Merge( min );
		}
	}

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

	m_lightProjection[cascadeLevel] = XMMatrixOrthographicOffCenterLH( frustumMin.x, frustumMax.x,
																				frustumMin.y, frustumMax.y,
																				casterMin.z, frustumMax.z );

	m_lightView[cascadeLevel] = XMMatrixMultiply( view.GetViewMatrix( ), lightView );
}

void CShadowManager::BuildPSMProjectionMatrix( CGameLogic& gameLogic, int cascadeLevel, float zClipNear, float zClipFar )
{
	using namespace DirectX;

	zClipNear = max( zClipNear, m_receiversNear );
	zClipFar = min( zClipFar, m_receiversFar );

	std::vector<CAaboundingbox> clipedResivePoints;
	clipedResivePoints.reserve( m_shadowReceiverPoints.size( ) );

	for ( const auto& aabb : m_shadowReceiverPoints )
	{
		const CXMFLOAT3& max = aabb.GetMax( );
		const CXMFLOAT3& min = aabb.GetMin( );

		if ( min.z < zClipFar && max.z > zClipNear )
		{
			clipedResivePoints.emplace_back( aabb );
		}
	}

	const CRenderView& view = gameLogic.GetView( );

	//float infinity = m_receiversFar / ( m_receiversFar - m_receiversNear );
	//
	//const float Z_EPSILON = 0.0001f;
	//if ( m_isSlideBack && ( infinity <= m_minInfinityZ ) )
	//{
	//	m_slideBack = m_minInfinityZ * ( m_receiversFar - m_receiversNear ) - m_receiversFar + Z_EPSILON;
	//	m_receiversFar += m_slideBack;
	//	m_receiversNear += m_slideBack;
	//}

	float infinity = zClipFar / ( zClipFar - zClipNear );

	const float Z_EPSILON = 0.0001f;
	if ( m_isSlideBack && ( infinity <= m_minInfinityZ ) )
	{
		m_slideBack = m_minInfinityZ * ( zClipFar - zClipNear ) - zClipFar + Z_EPSILON;
		zClipFar += m_slideBack;
		zClipNear += m_slideBack;
	}

	CXMFLOAT4X4 virtualCameraView = XMMatrixTranslation( 0.f, 0.f, m_slideBack );
	CXMFLOAT4X4 virtualCameraProjection;

	if ( m_isSlideBack )
	{
		if ( m_isUnitClipCube )
		{
			//CBoundingCone cone( m_shadowReceiverPoints, virtualCameraView, CXMFLOAT3( 0.f, 0.f, 0.f ), CXMFLOAT3( 0.f, 0.f, 1.f ) );
			CBoundingCone cone( clipedResivePoints, virtualCameraView, CXMFLOAT3( 0.f, 0.f, 0.f ), CXMFLOAT3( 0.f, 0.f, 1.f ) );
			//virtualCameraProjection = XMMatrixPerspectiveLH( 2.f * tanf( cone.GetFovX( ) ) * m_receiversNear, 2.f * tanf( cone.GetFovY() ) * m_receiversNear, m_receiversNear, m_receiversFar );
			virtualCameraProjection = XMMatrixPerspectiveLH( 2.f * tanf( cone.GetFovX( ) ) * zClipNear, 2.f * tanf( cone.GetFovY( ) ) * zClipNear, zClipNear, zClipFar );
		}
		else
		{
			float viewHeight = view.GetFar( ) * tanf( 0.5f * view.GetFov( ) );
			float viewWidth = view.GetFov( ) * viewHeight;

			float halfFovy = atanf( viewHeight / ( view.GetFar() + m_slideBack ) ) ;
			float halfFovx = atanf( viewWidth / ( view.GetFar() + m_slideBack ) );

			//virtualCameraProjection = XMMatrixPerspectiveLH( 2.f * tanf( halfFovx ) * m_receiversNear, 2.f * tanf( halfFovy ) * m_receiversNear, m_receiversNear, m_receiversFar );
			virtualCameraProjection = XMMatrixPerspectiveLH( 2.f * tanf( halfFovx ) * zClipNear, 2.f * tanf( halfFovy ) * zClipNear, zClipNear, zClipFar );
		}
	}
	else
	{
		virtualCameraView = XMMatrixIdentity( );
		//virtualCameraProjection = XMMatrixPerspectiveFovLH( view.GetFov( ), view.GetAspect( ), m_receiversNear, m_receiversFar );
		virtualCameraProjection = XMMatrixPerspectiveFovLH( view.GetFov( ), view.GetAspect( ), zClipNear, zClipFar );
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
				//viewCone = CBoundingCone( m_shadowReceiverPoints, eyeToPostProjectiveVirtualCamera, ppLightPos );
				viewCone = CBoundingCone( clipedResivePoints, eyeToPostProjectiveVirtualCamera, ppLightPos );
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
			lightProjection = XMMatrixPerspectiveLH_NoAssert( 2.f * tanf( viewCone.GetFovX( ) ) * -fNear, 2.f * tanf( viewCone.GetFovY( ) ) * -fNear, -fNear, fNear );
		}
		else
		{
			float fFovy;
			float fAspect;
			float fNear;
			float fFar;

			if ( m_isUnitClipCube )
			{
				//CBoundingCone viewCone( m_shadowReceiverPoints, eyeToPostProjectiveVirtualCamera, ppLightPos );
				CBoundingCone viewCone( clipedResivePoints, eyeToPostProjectiveVirtualCamera, ppLightPos );
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


	m_lightView[cascadeLevel] = XMMatrixMultiply( view.GetViewMatrix( ), virtualCameraView );
	m_lightView[cascadeLevel] = XMMatrixMultiply( m_lightView[cascadeLevel], virtualCameraProjection );
	m_lightView[cascadeLevel] = XMMatrixMultiply( m_lightView[cascadeLevel], lightView );
	m_lightProjection[cascadeLevel] = lightProjection;
}

void CShadowManager::BuildLSPSMProjectionMatrix( CGameLogic& gameLogic, int cascadeLevel, float zClipNear, float zClipFar )
{
	if ( abs( m_cosGamma ) >= 0.999f )
	{
		BuildOrthoShadowProjectionMatrix( gameLogic, 0, -FLT_MAX, FLT_MAX );
	}
	else
	{
		std::vector<CXMFLOAT3> bodyB;
		bodyB.reserve( m_shadowCasterPoints.size( ) * 8 + 8 );

		const CRenderView& view = gameLogic.GetView( );

		//CFrustum eyeFrustum( view.GetProjectionMatrix( ) );
		//const CXMFLOAT3 ( &frustumVertices )[8] = eyeFrustum.GetVertices( );
		//for ( int i = 0; i < 8; ++i )
		//{
		//	bodyB.emplace_back( frustumVertices[i] );
		//}

		for ( const auto& box : m_shadowCasterPoints )
		{
			const CXMFLOAT3& max = box.GetMax( );
			const CXMFLOAT3& min = box.GetMin( );

			if ( min.z < zClipFar && max.z > zClipNear )
			{
				for ( int i = 0; i < 8; ++i )
				{
					bodyB.emplace_back( box.Point( i ) );
				}
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

		float nOpt0 = m_receiversNear + sqrtf( m_receiversNear * m_receiversFar );

		float zNear = view.GetNear( );
		float zFar = view.GetFar( );
		float nOpt1 = zNear + sqrtf( zNear * zFar );

		float lspsmNopt = ( ( nOpt0 + nOpt1 ) / ( 2.f * sinGamma ) ) + 0.1f;

		float nOpt = 0.1f + m_nOptWeight * ( lspsmNopt - 0.1f );

		lightSpaceOrigin.z = lightSpaceBox.GetMin( ).z - nOpt;

		float maxX = -FLT_MAX, maxY = -FLT_MAX, maxZ = -FLT_MAX;

		CXMFLOAT3 tmp;
		for ( const auto& point : bodyB )
		{
			tmp = point - lightSpaceOrigin;

			maxX = max( maxX, abs( tmp.x / tmp.z ) );
			maxY = max( maxY, abs( tmp.y / tmp.z ) );
			maxZ = max( maxZ, tmp.z );
		}

		// float fovX = XMConvertToDegrees( atanf(maxX) );
		// float fovY = XMConvertToDegrees( atanf(maxY) );

		CXMFLOAT4X4 translate = XMMatrixTranslation( -lightSpaceOrigin.x, -lightSpaceOrigin.y, -lightSpaceOrigin.z );
		CXMFLOAT4X4 perspective = XMMatrixPerspectiveLH( 2.f * maxX * nOpt, 2.f * maxY * nOpt, nOpt, maxZ );

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
		m_lightView[cascadeLevel] = XMMatrixMultiply( view.GetViewMatrix( ), lightSpaceBasis );

		m_lightProjection[cascadeLevel] = ortho;

		if ( m_isUnitClipCube && ( m_shadowReceiverPoints.size() > 0 ) )
		{
			std::vector<CXMFLOAT3> receiverPoints;
			receiverPoints.reserve( m_shadowReceiverPoints.size( ) * 8 );

			for ( const auto& box : m_shadowReceiverPoints )
			{
				const CXMFLOAT3& max = box.GetMax( );
				const CXMFLOAT3& min = box.GetMin( );

				if ( min.z < zClipFar && max.z > zClipNear )
				{
					for ( int i = 0; i < 8; ++i )
					{
						receiverPoints.emplace_back( box.Point( i ) );
					}
				}
			}

			XMVector3TransformCoordStream( receiverPoints.data( ), sizeof( CXMFLOAT3 ), receiverPoints.data( ), sizeof( CXMFLOAT3 ), receiverPoints.size( ), lightSpaceBasis );

			CAaboundingbox receiverBox( receiverPoints );
			
			float minX = min( -1.f, receiverBox.GetMin().x );
			float minY = min( -1.f, receiverBox.GetMin( ).y );
			maxX = max( 1.f, receiverBox.GetMax( ).x );
			maxY = max( 1.f, receiverBox.GetMax( ).y );

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

				m_lightProjection[cascadeLevel] = XMMatrixMultiply( m_lightProjection[cascadeLevel], clip );
			}
		}
	}
}
