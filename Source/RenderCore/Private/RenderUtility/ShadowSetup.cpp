#include "stdafx.h"
#include "ShadowSetup.h"

#include "Math/CXMFloat.h"
#include "Physics/Aaboundingbox.h"
#include "Physics/Ray.h"
#include "Proxies/LightProxy.h"
#include "RenderView.h"
#include "Scene/LightSceneInfo.h"
#include "Scene/ShadowInfo.h"

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

	for ( uint32 cascadeLevel = 0; cascadeLevel < CascadeShadowSetting::MAX_CASCADE_NUM; ++cascadeLevel )
	{
		CascadeShadowSetting& cascadeSetting = shadowInfo.CascadeSetting( );

		float zClipNear = cascadeSetting.m_zClipNear[cascadeLevel];
		float zClipFar = cascadeSetting.m_zClipFar[cascadeLevel];

		CAaboundingbox frustumAABB;
		if ( true /*m_isUnitClipCube*/ )
		{
			for ( const BoxSphereBounds& bounds : shadowInfo.ShadowReceiversViewSpaceBounds( ) )
			{
				CXMFLOAT3 max = bounds.Origin( ) + bounds.HalfSize( );
				CXMFLOAT3 min = bounds.Origin( ) - bounds.HalfSize( );

				min.z = std::max( zClipNear, min.z );
				max.z = std::min( zClipFar, max.z );

				frustumAABB.Merge( max );
				frustumAABB.Merge( min );
			}
		}
		else
		{
			float height = view.m_fov;
			float width = height * view.m_aspect;
			float zNear = std::max( view.m_nearPlaneDistance, zClipNear );
			float zFar = std::min( view.m_farPlaneDistance, zClipFar );

			frustumAABB.SetMax( width * zFar, height * zFar, zFar );
			frustumAABB.SetMin( -width * zFar, -height * zFar, zNear );
		}

		CAaboundingbox casterAABB;
		for ( const BoxSphereBounds& bounds : shadowInfo.ShadowCastersViewSpaceBounds( ) )
		{
			CXMFLOAT3 max = bounds.Origin( ) + bounds.HalfSize( );
			CXMFLOAT3 min = bounds.Origin( ) - bounds.HalfSize( );

			min.z = std::max( zClipNear, min.z );
			max.z = std::min( zClipFar, max.z );

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

		shadowInfo.ShadowViewProjections( )[cascadeLevel] = XMMatrixMultiply( XMMatrixMultiply( viewMatrix, lightView ), shadowProjection );
	}
}