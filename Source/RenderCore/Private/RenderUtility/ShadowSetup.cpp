#include "stdafx.h"
#include "ShadowSetup.h"

#include "Math/CXMFloat.h"
#include "Physics/Aaboundingbox.h"
#include "Physics/BoundingCone.h"
#include "Physics/Frustum.h"
#include "Physics/Ray.h"
#include "Proxies/LightProxy.h"
#include "RenderView.h"
#include "Scene/LightSceneInfo.h"
#include "Scene/ShadowInfo.h"

namespace
{
	CXMFLOAT4X4 CreateCropMatrix( const BoxSphereBounds& bounds )
	{
		CXMFLOAT3 max = bounds.Origin( ) + bounds.HalfSize( );
		CXMFLOAT3 min = bounds.Origin( ) - bounds.HalfSize( );

		float scaleX = 2.0f / ( max.x - min.x );
		float scaleY = 2.0f / ( max.y - min.y );
		float scaleZ = 1.0f / ( max.z - min.z );

		float offsetX = -0.5f * ( max.x + min.x ) * scaleX;
		float offsetY = -0.5f * ( max.y + min.y ) * scaleY;
		float offsetZ = -min.z * scaleZ;

		scaleX = std::max( 1.f, scaleX );
		scaleY = std::max( 1.f, scaleY );

		return CXMFLOAT4X4( scaleX, 0.f, 0.f, 0.f,
							0.f, scaleY, 0.f, 0.f,
							0.f, 0.f, scaleZ, 0.f,
							offsetX, offsetY, offsetZ, 1.f);
	}

	BoxSphereBounds CalcFrustumBounds( const CXMFLOAT3& viewOrigin, const CXMFLOAT3X3& viewAxis, float nearPlane, float farPlane, float aspect, float fov, const CXMFLOAT4X4& shadowMat )
	{
		float nearPlaneHalfHeight = std::tanf( fov * 0.5f ) * nearPlane;
		float nearPlaneHalfWidth = nearPlaneHalfHeight * aspect;

		float farPlaneHalfHeight = std::tanf( fov * 0.5f ) * farPlane;
		float farPlaneHalfWidth = farPlaneHalfHeight * aspect;

		const CXMFLOAT3& look = viewAxis[2];
		const CXMFLOAT3& up = viewAxis[1];
		const CXMFLOAT3& right = viewAxis[0];

		CXMFLOAT3 nearPlaneCenter = viewOrigin + look * nearPlane;
		CXMFLOAT3 farPlaneCenter = viewOrigin + look * farPlane;

		CXMFLOAT3 corners[8] = {
			{ nearPlaneCenter - CXMFLOAT3( right * nearPlaneHalfWidth ) - CXMFLOAT3( up * nearPlaneHalfHeight ) },
			{ nearPlaneCenter - CXMFLOAT3( right * nearPlaneHalfWidth ) + CXMFLOAT3( up * nearPlaneHalfHeight ) },
			{ nearPlaneCenter + CXMFLOAT3( right * nearPlaneHalfWidth ) + CXMFLOAT3( up * nearPlaneHalfHeight ) },
			{ nearPlaneCenter + CXMFLOAT3( right * nearPlaneHalfWidth ) - CXMFLOAT3( up * nearPlaneHalfHeight ) },
			{ farPlaneCenter - CXMFLOAT3( right * farPlaneHalfWidth ) - CXMFLOAT3( up * farPlaneHalfHeight ) },
			{ farPlaneCenter - CXMFLOAT3( right * farPlaneHalfWidth ) + CXMFLOAT3( up * farPlaneHalfHeight ) },
			{ farPlaneCenter + CXMFLOAT3( right * farPlaneHalfWidth ) + CXMFLOAT3( up * farPlaneHalfHeight ) },
			{ farPlaneCenter + CXMFLOAT3( right * farPlaneHalfWidth ) - CXMFLOAT3( up * farPlaneHalfHeight ) }
		};

		for ( CXMFLOAT3& corner : corners )
		{
			corner = DirectX::XMVector3TransformCoord( corner, shadowMat );
		}

		return BoxSphereBounds( corners, std::extent_v<decltype( corners )> );
	}

	void SplitShadowProjectionMatrix( ShadowInfo& shadowInfo, const RenderView& view, const CXMFLOAT4X4& shadowViewProjMat, const CXMFLOAT4X4& shadowMat )
	{
		CalculateSplitPositions( shadowInfo, shadowInfo.SubjectFar( ) );

		CAaboundingbox casterAABB;
		for ( const BoxSphereBounds& bounds : shadowInfo.ShadowCastersViewSpaceBounds( ) )
		{
			CAaboundingbox aabb( bounds );
			TransformAABB( aabb, aabb, shadowViewProjMat );

			casterAABB.Merge( aabb.GetMin( ) );
			casterAABB.Merge( aabb.GetMax( ) );
		}

		CascadeShadowSetting& cascadeSetting = shadowInfo.CascadeSetting( );
		const float* splitDistance = cascadeSetting.m_splitDistance;
		float extend = 10.f / shadowInfo.ShadowMapSize( ).first;
		for ( uint32 cascadeLevel = 0; cascadeLevel < CascadeShadowSetting::MAX_CASCADE_NUM; ++cascadeLevel )
		{
			BoxSphereBounds bounds = CalcFrustumBounds( view.m_viewOrigin, view.m_viewAxis, splitDistance[cascadeLevel], splitDistance[cascadeLevel + 1], view.m_aspect, view.m_fov, shadowMat );

			CXMFLOAT3 frustumMinMax[2] = { bounds.Origin( ) - bounds.HalfSize( ), bounds.Origin( ) + bounds.HalfSize( ) };

			frustumMinMax[0].x = std::max( frustumMinMax[0].x, casterAABB.GetMin( ).x ) - extend;
			frustumMinMax[0].y = std::max( frustumMinMax[0].y, casterAABB.GetMin( ).y ) - extend;
			frustumMinMax[0].z = casterAABB.GetMin( ).z - extend;
			frustumMinMax[1].x = std::min( frustumMinMax[1].x, casterAABB.GetMax( ).x ) + extend;
			frustumMinMax[1].y = std::min( frustumMinMax[1].y, casterAABB.GetMax( ).y ) + extend;
			frustumMinMax[1].z = std::min( frustumMinMax[1].z, casterAABB.GetMax( ).z ) + extend;

			bounds = BoxSphereBounds( frustumMinMax, 2 );
			CXMFLOAT4X4 cropMat = CreateCropMatrix( bounds );

			shadowInfo.ShadowViewProjections( )[cascadeLevel] = XMMatrixMultiply( shadowMat, cropMat );
		}
	}
}

void BuildOrthoShadowProjectionMatrix( ShadowInfo& shadowInfo )
{
	using namespace DirectX;

	const LightSceneInfo* lightSceneInfo = shadowInfo.GetLightSceneInfo( );
	LightProperty lightProperty = lightSceneInfo->Proxy( )->GetLightProperty( );

	const RenderView& view = *shadowInfo.View( );
	auto viewMatrix = XMMatrixLookToLH( view.m_viewOrigin,
										view.m_viewAxis[2],
										view.m_viewAxis[1] );

	CXMFLOAT3 viewLightDir = XMVector3TransformNormal( lightProperty.m_direction, viewMatrix );
	viewLightDir = XMVector3Normalize( viewLightDir );

	CAaboundingbox frustumAABB;
	if ( true /*m_isUnitClipCube*/ )
	{
		for ( const BoxSphereBounds& bounds : shadowInfo.ShadowReceiversViewSpaceBounds( ) )
		{
			CXMFLOAT3 max = bounds.Origin( ) + bounds.HalfSize( );
			CXMFLOAT3 min = bounds.Origin( ) - bounds.HalfSize( );

			frustumAABB.Merge( max );
			frustumAABB.Merge( min );
		}
	}
	else
	{
		float height = view.m_fov;
		float width = height * view.m_aspect;
		float zNear = view.m_nearPlaneDistance;
		float zFar = view.m_farPlaneDistance;

		frustumAABB.SetMax( width * zFar, height * zFar, zFar );
		frustumAABB.SetMin( -width * zFar, -height * zFar, zNear );
	}

	CAaboundingbox casterAABB;
	for ( const BoxSphereBounds& bounds : shadowInfo.ShadowCastersViewSpaceBounds( ) )
	{
		CXMFLOAT3 max = bounds.Origin( ) + bounds.HalfSize( );
		CXMFLOAT3 min = bounds.Origin( ) - bounds.HalfSize( );

		casterAABB.Merge( max );
		casterAABB.Merge( min );
	}

	CXMFLOAT3 center = frustumAABB.Centroid( );

	CXMFLOAT3 rayOrigin = -viewLightDir * view.m_farPlaneDistance;
	rayOrigin += center;

	CRay ray( rayOrigin, viewLightDir );
	float t = casterAABB.Intersect( ray );

	CXMFLOAT3 lightPosition = lightProperty.m_position;
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

	CXMFLOAT4X4 shadowProjection = XMMatrixOrthographicOffCenterLH( frustumMin.x, frustumMax.x,
		frustumMin.y, frustumMax.y,
		casterMin.z, frustumMax.z );

	CXMFLOAT4X4 shadowViewProjMat = XMMatrixMultiply( lightView, shadowProjection );
	CXMFLOAT4X4 shadowMat = XMMatrixMultiply( viewMatrix, shadowViewProjMat );

	SplitShadowProjectionMatrix( shadowInfo, view, shadowViewProjMat, shadowMat );
}

void BuildPSMProjectionMatrix( ShadowInfo& shadowInfo )
{
	using namespace DirectX;

	const LightSceneInfo* lightSceneInfo = shadowInfo.GetLightSceneInfo( );
	LightProperty lightProperty = lightSceneInfo->Proxy( )->GetLightProperty( );

	const RenderView& view = *shadowInfo.View( );
	auto viewMatrix = XMMatrixLookToLH( view.m_viewOrigin,
										view.m_viewAxis[2],
										view.m_viewAxis[1] );

	std::vector<CXMFLOAT3> clipedResivePoints;
	clipedResivePoints.reserve( shadowInfo.ShadowReceiversViewSpaceBounds( ).size( ) * 8 );

	for ( const BoxSphereBounds& bounds : shadowInfo.ShadowReceiversViewSpaceBounds( ) )
	{
		CXMFLOAT3 max = bounds.Origin( ) + bounds.HalfSize( );
		CXMFLOAT3 min = bounds.Origin( ) - bounds.HalfSize( );

		for ( uint32 i = 0; i < 8; ++i )
		{
			clipedResivePoints.emplace_back( ( i & 1 ) ? min.x : max.x, ( i & 2 ) ? min.y : max.y, ( i & 4 ) ? min.z : max.z );
		}
	}

	float zClipFar = view.m_farPlaneDistance;
	float zClipNear = view.m_nearPlaneDistance;
	float infinity = zClipFar / ( zClipFar - zClipNear );
	float slideBack = 0;

	constexpr float Z_EPSILON = 0.0001f;
	constexpr float MinInfinityZ = 1.5f;
	if ( true /*m_isSlideBack*/ && ( infinity <= MinInfinityZ ) )
	{
		slideBack = MinInfinityZ * ( zClipFar - zClipNear ) - zClipFar + Z_EPSILON;
		zClipFar += slideBack;
		zClipNear += slideBack;
	}

	CXMFLOAT4X4 virtualCameraView = XMMatrixTranslation( 0.f, 0.f, slideBack );
	CXMFLOAT4X4 virtualCameraProjection;

	if ( true /*m_isSlideBack*/ )
	{
		if ( true /*m_isUnitClipCube*/ )
		{
			CBoundingCone cone( clipedResivePoints, virtualCameraView, CXMFLOAT3( 0.f, 0.f, 0.f ), CXMFLOAT3( 0.f, 0.f, 1.f ) );
			virtualCameraProjection = XMMatrixPerspectiveLH( 2.f * tanf( cone.GetFovX( ) ) * zClipNear, 2.f * tanf( cone.GetFovY( ) ) * zClipNear, zClipNear, zClipFar );
		}
		else
		{
			float viewHeight = view.m_fov * tanf( 0.5f * view.m_fov );
			float viewWidth = view.m_fov * viewHeight;

			float halfFovy = atanf( viewHeight / ( view.m_farPlaneDistance + slideBack ) );
			float halfFovx = atanf( viewWidth / ( view.m_farPlaneDistance + slideBack ) );

			virtualCameraProjection = XMMatrixPerspectiveLH( 2.f * tanf( halfFovx ) * zClipNear, 2.f * tanf( halfFovy ) * zClipNear, zClipNear, zClipFar );
		}
	}
	else
	{
		virtualCameraView = XMMatrixIdentity( );
		virtualCameraProjection = XMMatrixPerspectiveFovLH( view.m_fov, view.m_aspect, zClipNear, zClipFar );
	}

	CXMFLOAT4 lightPos = XMVector3TransformNormal( -lightProperty.m_direction, viewMatrix );
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

		CXMFLOAT3 cubeCenter = ppUnitBox.Centroid( );

		CXMFLOAT3 rayOrigin = ppLightDir * 2.f;
		rayOrigin += cubeCenter;

		CRay ray( rayOrigin, -ppLightDir );
		float t = ppUnitBox.Intersect( ray );

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
			if ( false /*m_isUnitClipCube*/ )
			{
				viewCone = CBoundingCone( clipedResivePoints, eyeToPostProjectiveVirtualCamera, ppLightPos );
			}
			else
			{
				std::vector<CXMFLOAT3> ndcBox;
				ndcBox.reserve( 8 );
				for ( uint32 i = 0; i < 8; ++i )
				{
					ndcBox.emplace_back( ( i & 1 ) ? -1.f : 1.f, ( i & 2 ) ? -1.f : 1.f, ( i & 4 ) ? 0.f : 1.f );
				}
				viewCone = CBoundingCone( ndcBox, XMMatrixIdentity( ), ppLightPos );
			}
			lightView = viewCone.GetLookAt( );

			float fNear = std::max( 0.001f, viewCone.GetNear( ) * 0.3f );
			float width = 2.f * tanf( viewCone.GetFovX( ) ) * -fNear;
			float height = 2.f * tanf( viewCone.GetFovY( ) ) * -fNear;

			lightProjection = CXMFLOAT4X4( ( -2.f * fNear ) / width, 0, 0, 0,
				0, ( -2.f * fNear ) / height, 0, 0,
				0, 0, fNear / ( 2.f * fNear ), 1,
				0, 0, ( fNear * fNear ) / ( 2.f * fNear ), 0 );
		}
		else
		{
			float fFovy;
			float fAspect;
			float fNear;
			float fFar;

			if ( false /*m_isUnitClipCube*/ )
			{
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

			fNear = std::max( 0.001f, fNear );
			lightProjection = XMMatrixPerspectiveFovLH( fFovy, fAspect, fNear, fFar );
		}
	}

	CXMFLOAT4X4 shadowViewProjMat = XMMatrixMultiply( virtualCameraView, virtualCameraProjection );
	shadowViewProjMat = XMMatrixMultiply( shadowViewProjMat, lightView );
	shadowViewProjMat = XMMatrixMultiply( shadowViewProjMat, lightProjection );
	CXMFLOAT4X4 shadowMat = XMMatrixMultiply( viewMatrix, shadowViewProjMat );

	SplitShadowProjectionMatrix( shadowInfo, view, shadowViewProjMat, shadowMat );
}

void BuildLSPSMProjectionMatrix( ShadowInfo& shadowInfo )
{
	using namespace DirectX;

	const LightSceneInfo* lightSceneInfo = shadowInfo.GetLightSceneInfo( );
	LightProperty lightProperty = lightSceneInfo->Proxy( )->GetLightProperty( );

	const RenderView& view = *shadowInfo.View( );
	CXMFLOAT4X4 viewMatrix = XMMatrixLookToLH( view.m_viewOrigin,
												view.m_viewAxis[2],
												view.m_viewAxis[1] );

	float cosGamma = XMVectorGetX( XMVector3Dot( -lightProperty.m_direction, CXMFLOAT3( viewMatrix._13, viewMatrix._23, viewMatrix._33 ) ) );

	if ( abs( cosGamma ) >= 0.999f )
	{
		BuildOrthoShadowProjectionMatrix( shadowInfo );
	}
	else
	{
		std::vector<CXMFLOAT3> bodyB;
		bodyB.reserve( shadowInfo.ShadowCastersViewSpaceBounds( ).size( ) * 8 + 8 );

		for ( const BoxSphereBounds& bounds : shadowInfo.ShadowCastersViewSpaceBounds( ) )
		{
			CXMFLOAT3 max = bounds.Origin( ) + bounds.HalfSize( );
			CXMFLOAT3 min = bounds.Origin( ) - bounds.HalfSize( );

			for ( uint32 i = 0; i < 8; ++i )
			{
				bodyB.emplace_back( ( i & 1 ) ? min.x : max.x, ( i & 2 ) ? min.y : max.y, ( i & 4 ) ? min.z : max.z );
			}
		}

		CXMFLOAT3 upVector = XMVector3Normalize( XMVector3TransformNormal( -lightProperty.m_direction, viewMatrix ) );

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

		CAaboundingbox lightSpaceBox( bodyB.data( ), static_cast<uint32>( bodyB.size( ) ) );
		CXMFLOAT3 lightSpaceOrigin = lightSpaceBox.Centroid( );
		float sinGamma = sqrtf( 1.f - cosGamma * cosGamma );

		float receiversNear = -FLT_MAX;
		float receiversFar = FLT_MAX;
		for ( const BoxSphereBounds& bounds : shadowInfo.ShadowReceiversViewSpaceBounds( ) )
		{
			CXMFLOAT3 max = bounds.Origin( ) + bounds.HalfSize( );
			CXMFLOAT3 min = bounds.Origin( ) - bounds.HalfSize( );

			receiversNear = std::max( receiversNear, min.z );
			receiversFar = std::min( receiversFar, max.z );
		}

		float nOpt0 = receiversNear + sqrtf( receiversNear * receiversFar );

		float zNear = view.m_nearPlaneDistance;
		float zFar = view.m_farPlaneDistance;
		float nOpt1 = zNear + sqrtf( zNear * zFar );

		float lspsmNopt = ( ( nOpt0 + nOpt1 ) / ( 2.f * sinGamma ) ) + 0.1f;

		constexpr float OptWeight = 1.f;
		float nOpt = 0.1f + OptWeight * ( lspsmNopt - 0.1f );

		lightSpaceOrigin.z = lightSpaceBox.GetMin( ).z - nOpt;

		float maxX = -FLT_MAX, maxY = -FLT_MAX, maxZ = -FLT_MAX;

		CXMFLOAT3 tmp;
		for ( const auto& point : bodyB )
		{
			tmp = point - lightSpaceOrigin;

			maxX = std::max( maxX, abs( tmp.x / tmp.z ) );
			maxY = std::max( maxY, abs( tmp.y / tmp.z ) );
			maxZ = std::max( maxZ, tmp.z );
		}

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
		CXMFLOAT4X4 lightProjection = ortho;

		if ( false /*m_isUnitClipCube*/ && ( shadowInfo.ShadowReceiversViewSpaceBounds( ).size( ) > 0 ) )
		{
			std::vector<CXMFLOAT3> receiverPoints;
			receiverPoints.reserve( shadowInfo.ShadowReceiversViewSpaceBounds( ).size( ) * 8 );

			for ( const BoxSphereBounds& bounds : shadowInfo.ShadowReceiversViewSpaceBounds( ) )
			{
				CXMFLOAT3 max = bounds.Origin( ) + bounds.HalfSize( );
				CXMFLOAT3 min = bounds.Origin( ) - bounds.HalfSize( );

				for ( uint32 i = 0; i < 8; ++i )
				{
					receiverPoints.emplace_back( ( i & 1 ) ? min.x : max.x, ( i & 2 ) ? min.y : max.y, ( i & 4 ) ? min.z : max.z );
				}
			}

			XMVector3TransformCoordStream( receiverPoints.data( ), sizeof( CXMFLOAT3 ), receiverPoints.data( ), sizeof( CXMFLOAT3 ), receiverPoints.size( ), lightSpaceBasis );

			CAaboundingbox receiverBox( receiverPoints.data( ), static_cast<uint32>( receiverPoints.size( ) ) );

			float minX = std::min( -1.f, receiverBox.GetMin( ).x );
			float minY = std::min( -1.f, receiverBox.GetMin( ).y );
			maxX = std::max( 1.f, receiverBox.GetMax( ).x );
			maxY = std::max( 1.f, receiverBox.GetMax( ).y );

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

				lightProjection = XMMatrixMultiply( lightProjection, clip );
			}
		}

		CXMFLOAT4X4 shadowViewProjMat = XMMatrixMultiply( lightSpaceBasis, lightProjection );
		CXMFLOAT4X4 shadowMat = XMMatrixMultiply( viewMatrix, shadowViewProjMat );

		SplitShadowProjectionMatrix( shadowInfo, view, shadowViewProjMat, shadowMat );
	}
}

void CalculateSplitPositions( ShadowInfo& shadowInfo, float casterFar, float lamda )
{
	const RenderView& view = *shadowInfo.View( );

	float invM = 1.f / CascadeShadowSetting::MAX_CASCADE_NUM;
	float farPlaneDistance = std::min( view.m_farPlaneDistance, casterFar );
	float fDivN = farPlaneDistance / view.m_nearPlaneDistance;
	float fSubN = farPlaneDistance - view.m_nearPlaneDistance;

	CascadeShadowSetting& cascadeSetting = shadowInfo.CascadeSetting( );
	for ( int i = 0; i <= CascadeShadowSetting::MAX_CASCADE_NUM; ++i )
	{
		float CiLog = view.m_nearPlaneDistance * std::powf( fDivN, invM * i );
		float CiUni = view.m_nearPlaneDistance * fSubN * i * invM;
		cascadeSetting.m_splitDistance[i] = lamda * CiLog + CiUni * ( 1.f - lamda );
	}

	cascadeSetting.m_splitDistance[0] = view.m_nearPlaneDistance;
	cascadeSetting.m_splitDistance[CascadeShadowSetting::MAX_CASCADE_NUM] = farPlaneDistance;
}
