#include "stdafx.h"
#include "ShadowSetup.h"

#include "Math/TransformationMatrix.h"
#include "Math/Vector.h"
#include "Math/Vector4.h"
#include "Physics/AxisAlignedBox.h"
#include "Physics/BoundingCone.h"
#include "Physics/Frustum.h"
#include "Physics/Ray.h"
#include "Proxies/LightProxy.h"
#include "RenderView.h"
#include "Scene/LightSceneInfo.h"
#include "Scene/ShadowInfo.h"

using ::DirectX::XMConvertToRadians;
using ::rendercore::CascadeShadowSetting;
using ::rendercore::RenderView;
using ::rendercore::ShadowInfo;

namespace
{
	Matrix CreateCropMatrix( const BoxSphereBounds& bounds )
	{
		Vector max = bounds.Origin() + bounds.HalfSize();
		Vector min = bounds.Origin() - bounds.HalfSize();

		float scaleX = 2.0f / ( max.x - min.x );
		float scaleY = 2.0f / ( max.y - min.y );
		float scaleZ = 1.0f / ( max.z - min.z );

		float offsetX = -0.5f * ( max.x + min.x ) * scaleX;
		float offsetY = -0.5f * ( max.y + min.y ) * scaleY;
		float offsetZ = -min.z * scaleZ;

		scaleX = std::max( 1.f, scaleX );
		scaleY = std::max( 1.f, scaleY );

		return Matrix( scaleX, 0.f, 0.f, 0.f,
			0.f, scaleY, 0.f, 0.f,
			0.f, 0.f, scaleZ, 0.f,
			offsetX, offsetY, offsetZ, 1.f );
	}

	BoxSphereBounds CalcFrustumBounds( const Vector& viewOrigin, const BasisVectorMatrix& viewAxis, float nearPlane, float farPlane, float aspect, float fov, const Matrix& shadowMat )
	{
		float nearPlaneHalfHeight = std::tanf( fov * 0.5f ) * nearPlane;
		float nearPlaneHalfWidth = nearPlaneHalfHeight * aspect;

		float farPlaneHalfHeight = std::tanf( fov * 0.5f ) * farPlane;
		float farPlaneHalfWidth = farPlaneHalfHeight * aspect;

		const Vector& look = viewAxis[2];
		const Vector& up = viewAxis[1];
		const Vector& right = viewAxis[0];

		Vector nearPlaneCenter = viewOrigin + look * nearPlane;
		Vector farPlaneCenter = viewOrigin + look * farPlane;

		Vector corners[8] = {
			{ nearPlaneCenter - right * nearPlaneHalfWidth - up * nearPlaneHalfHeight },
			{ nearPlaneCenter - right * nearPlaneHalfWidth + up * nearPlaneHalfHeight },
			{ nearPlaneCenter + right * nearPlaneHalfWidth + up * nearPlaneHalfHeight },
			{ nearPlaneCenter + right * nearPlaneHalfWidth - up * nearPlaneHalfHeight },
			{ farPlaneCenter - right * farPlaneHalfWidth - up * farPlaneHalfHeight },
			{ farPlaneCenter - right * farPlaneHalfWidth + up * farPlaneHalfHeight },
			{ farPlaneCenter + right * farPlaneHalfWidth + up * farPlaneHalfHeight },
			{ farPlaneCenter + right * farPlaneHalfWidth - up * farPlaneHalfHeight }
		};

		for ( Vector& corner : corners )
		{
			corner = shadowMat.TransformPosition( corner );
		}

		return BoxSphereBounds( corners, std::extent_v<decltype( corners )> );
	}

	void SplitShadowProjectionMatrix( ShadowInfo& shadowInfo, const RenderView& view, const Matrix& shadowViewProjMat, const Matrix& shadowMat )
	{
		CalculateSplitPositions( shadowInfo, shadowInfo.SubjectFar() );

		AxisAlignedBox casterAABB;
		for ( const BoxSphereBounds& bounds : shadowInfo.ShadowCastersViewSpaceBounds() )
		{
			AxisAlignedBox aabb( bounds );
			TransformAABB( aabb, aabb, shadowViewProjMat );

			casterAABB.Merge( aabb.GetMin() );
			casterAABB.Merge( aabb.GetMax() );
		}

		CascadeShadowSetting& cascadeSetting = shadowInfo.CascadeSetting();
		const float* splitDistance = cascadeSetting.m_splitDistance;
		float extend = 10.f / shadowInfo.ShadowMapSize().first;
		for ( uint32 cascadeLevel = 0; cascadeLevel < CascadeShadowSetting::MAX_CASCADE_NUM; ++cascadeLevel )
		{
			BoxSphereBounds bounds = CalcFrustumBounds( view.m_viewOrigin, view.m_viewAxis, splitDistance[cascadeLevel], splitDistance[cascadeLevel + 1], view.m_aspect, view.m_fov, shadowMat );

			Vector frustumMinMax[2] = { bounds.Origin() - bounds.HalfSize(), bounds.Origin() + bounds.HalfSize() };

			frustumMinMax[0].x = std::max( frustumMinMax[0].x, casterAABB.GetMin().x ) - extend;
			frustumMinMax[0].y = std::max( frustumMinMax[0].y, casterAABB.GetMin().y ) - extend;
			frustumMinMax[0].z = casterAABB.GetMin().z - extend;
			frustumMinMax[1].x = std::min( frustumMinMax[1].x, casterAABB.GetMax().x ) + extend;
			frustumMinMax[1].y = std::min( frustumMinMax[1].y, casterAABB.GetMax().y ) + extend;
			frustumMinMax[1].z = std::min( frustumMinMax[1].z, casterAABB.GetMax().z ) + extend;

			bounds = BoxSphereBounds( frustumMinMax, 2 );
			Matrix cropMat = CreateCropMatrix( bounds );

			shadowInfo.ShadowViewProjections()[cascadeLevel] = shadowMat * cropMat;
		}
	}
}

namespace rendercore
{
	void BuildOrthoShadowProjectionMatrix( ShadowInfo& shadowInfo )
	{
		if ( shadowInfo.ShadowReceiversViewSpaceBounds().size() == 0 )
		{
			return;
		}

		const LightSceneInfo* lightSceneInfo = shadowInfo.GetLightSceneInfo();
		LightProperty lightProperty = lightSceneInfo->Proxy()->GetLightProperty();

		const RenderView& view = *shadowInfo.View();
		auto viewMatrix = LookFromMatrix( view.m_viewOrigin,
			view.m_viewAxis[2],
			view.m_viewAxis[1] );

		auto viewLightDir = viewMatrix.TransformVector( lightProperty.m_direction );
		viewLightDir = viewLightDir.GetNormalized();

		AxisAlignedBox frustumAABB;
		if ( true /*m_isUnitClipCube*/ )
		{
			for ( const BoxSphereBounds& bounds : shadowInfo.ShadowReceiversViewSpaceBounds() )
			{
				Vector max = bounds.Origin() + bounds.HalfSize();
				Vector min = bounds.Origin() - bounds.HalfSize();

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

		AxisAlignedBox casterAABB;
		for ( const BoxSphereBounds& bounds : shadowInfo.ShadowCastersViewSpaceBounds() )
		{
			Vector max = bounds.Origin() + bounds.HalfSize();
			Vector min = bounds.Origin() - bounds.HalfSize();

			casterAABB.Merge( max );
			casterAABB.Merge( min );
		}

		const Vector& center = frustumAABB.Centroid();

		auto rayOrigin = -viewLightDir * view.m_farPlaneDistance;
		rayOrigin += center;

		CRay ray( rayOrigin, viewLightDir );
		float t = casterAABB.Intersect( ray );

		auto lightPosition = Vector( lightProperty.m_position );
		if ( t > 0.f )
		{
			Vector collsionPos = rayOrigin;
			collsionPos += viewLightDir * t;

			float distance = ( center - collsionPos ).Length();
			lightPosition = center;
			lightPosition -= viewLightDir * distance * 2.f;
		}
		else
		{
			return;
		}

		Vector axis = Vector::YAxisVector;
		if ( fabsf( viewLightDir | axis ) > 0.99f )
		{
			axis = Vector::ZAxisVector;
		}

		Matrix lightView = LookAtMatrix( lightPosition, center, axis );

		TransformAABB( frustumAABB, frustumAABB, lightView );
		TransformAABB( casterAABB, casterAABB, lightView );

		const Vector& frustumMin = frustumAABB.GetMin();
		const Vector& frustumMax = frustumAABB.GetMax();
		const Vector& casterMin = casterAABB.GetMin();

		Matrix shadowProjection = OrthoMatrix( frustumMin.x, frustumMax.x,
			frustumMin.y, frustumMax.y,
			casterMin.z, frustumMax.z );

		auto shadowViewProjMat = lightView * shadowProjection;
		auto shadowMat = viewMatrix * shadowViewProjMat;

		SplitShadowProjectionMatrix( shadowInfo, view, shadowViewProjMat, shadowMat );
	}

	void BuildPSMProjectionMatrix( ShadowInfo& shadowInfo )
	{
		const LightSceneInfo* lightSceneInfo = shadowInfo.GetLightSceneInfo();
		LightProperty lightProperty = lightSceneInfo->Proxy()->GetLightProperty();

		const RenderView& view = *shadowInfo.View();
		auto viewMatrix = LookFromMatrix( view.m_viewOrigin,
			view.m_viewAxis[2],
			view.m_viewAxis[1] );

		RenderThreadFrameData<Vector> clipedResivePoints;
		clipedResivePoints.reserve( shadowInfo.ShadowReceiversViewSpaceBounds().size() * 8 );

		for ( const BoxSphereBounds& bounds : shadowInfo.ShadowReceiversViewSpaceBounds() )
		{
			Vector max = bounds.Origin() + bounds.HalfSize();
			Vector min = bounds.Origin() - bounds.HalfSize();

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

		Matrix virtualCameraView = TranslationMatrix( 0.f, 0.f, slideBack );
		Matrix virtualCameraProjection;

		if ( true /*m_isSlideBack*/ )
		{
			if ( true /*m_isUnitClipCube*/ )
			{
				CBoundingCone cone( clipedResivePoints.data(), clipedResivePoints.size(), virtualCameraView, Vector::ZeroVector, Vector::ForwardVector );
				virtualCameraProjection = PerspectiveMatrix( { 2.f * tanf( cone.GetFovX() ) * zClipNear, 2.f * tanf( cone.GetFovY() ) * zClipNear }, zClipNear, zClipFar );
			}
			else
			{
				float viewHeight = view.m_fov * tanf( 0.5f * view.m_fov );
				float viewWidth = view.m_fov * viewHeight;

				float halfFovy = atanf( viewHeight / ( view.m_farPlaneDistance + slideBack ) );
				float halfFovx = atanf( viewWidth / ( view.m_farPlaneDistance + slideBack ) );

				virtualCameraProjection = PerspectiveMatrix( { 2.f * tanf( halfFovx ) * zClipNear, 2.f * tanf( halfFovy ) * zClipNear }, zClipNear, zClipFar );
			}
		}
		else
		{
			virtualCameraView = Matrix::Identity;
			virtualCameraProjection = PerspectiveMatrix( view.m_fov, view.m_aspect, zClipNear, zClipFar );
		}

		Vector4 lightPos = viewMatrix.TransformVector( -lightProperty.m_direction );
		lightPos = virtualCameraProjection.TransformVector( lightPos );

		Matrix lightView;
		Matrix lightProjection;

		if ( fabsf( lightPos.w ) <= 0.001f )
		{
			Vector ppLightDir( lightPos.x, lightPos.y, lightPos.z );
			ppLightDir = ppLightDir.GetNormalized();

			AxisAlignedBox ppUnitBox;
			ppUnitBox.SetMax( 1, 1, 1 );
			ppUnitBox.SetMin( -1, -1, 0 );

			Vector cubeCenter = ppUnitBox.Centroid();

			Vector rayOrigin = ppLightDir * 2.f;
			rayOrigin += cubeCenter;

			CRay ray( rayOrigin, -ppLightDir );
			float t = ppUnitBox.Intersect( ray );

			Vector lightPosition;
			if ( t > 0.f )
			{
				Vector collsionPos = rayOrigin;
				collsionPos -= ppLightDir * t;

				float distance = ( cubeCenter - collsionPos ).Length();
				lightPosition = cubeCenter;
				lightPosition += ppLightDir * distance * 2.f;
			}
			else
			{
				assert( false );
			}

			Vector axis = Vector::YAxisVector;
			if ( fabsf( ppLightDir | axis ) > 0.99f )
			{
				axis = Vector::ZAxisVector;
			}

			lightView = LookAtMatrix( lightPosition, cubeCenter, axis );

			TransformAABB( ppUnitBox, ppUnitBox, lightView );
			const Vector& unitBoxMin = ppUnitBox.GetMin();
			const Vector& unitBoxMax = ppUnitBox.GetMax();

			lightProjection = OrthoMatrix( unitBoxMin.x, unitBoxMax.x, unitBoxMin.y, unitBoxMax.y, unitBoxMin.z, unitBoxMax.z );
		}
		else
		{
			Vector ppLightPos = lightPos / lightPos.w;

			Matrix eyeToPostProjectiveVirtualCamera;
			eyeToPostProjectiveVirtualCamera = virtualCameraView * virtualCameraProjection;

			bool isShadowTestInverted = ( lightPos.w < 0.f );
			if ( isShadowTestInverted )
			{
				CBoundingCone viewCone;
				if ( false /*m_isUnitClipCube*/ )
				{
					viewCone = CBoundingCone( clipedResivePoints.data(), clipedResivePoints.size(), eyeToPostProjectiveVirtualCamera, ppLightPos );
				}
				else
				{
					RenderThreadFrameData<Vector> ndcBox;
					ndcBox.reserve( 8 );
					for ( uint32 i = 0; i < 8; ++i )
					{
						ndcBox.emplace_back( ( i & 1 ) ? -1.f : 1.f, ( i & 2 ) ? -1.f : 1.f, ( i & 4 ) ? 0.f : 1.f );
					}
					viewCone = CBoundingCone( ndcBox.data(), ndcBox.size(), Matrix::Identity, ppLightPos );
				}
				lightView = viewCone.GetLookAt();

				float fNear = std::max( 0.001f, viewCone.GetNear() * 0.3f );
				float width = 2.f * tanf( viewCone.GetFovX() ) * -fNear;
				float height = 2.f * tanf( viewCone.GetFovY() ) * -fNear;

				lightProjection = Matrix( ( -2.f * fNear ) / width, 0, 0, 0,
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
					CBoundingCone viewCone( clipedResivePoints.data(), clipedResivePoints.size(), eyeToPostProjectiveVirtualCamera, ppLightPos );
					lightView = viewCone.GetLookAt();

					fFovy = viewCone.GetFovY() * 2.f;
					fAspect = viewCone.GetFovX() / viewCone.GetFovY();
					fFar = viewCone.GetFar();
					fNear = viewCone.GetNear();
				}
				else
				{
					Vector lookAt = Vector( 0.f, 0.f, 0.5f ) - ppLightPos;

					float distance = lookAt.Length();
					lookAt /= distance;

					Vector axis = Vector::YAxisVector;
					if ( fabsf( lookAt | axis ) > 0.99f )
					{
						axis = Vector::ZAxisVector;
					}

					lightView = LookAtMatrix( ppLightPos, Vector( 0.f, 0.f, 0.5f ), axis );

					constexpr float ppCubeRadius = 1.5f;  // the post-projective view box radius is 1.5
					fFovy = 2.f * atanf( ppCubeRadius / distance );
					fAspect = 1.f;
					fNear = distance - 2.f * ppCubeRadius;
					fFar = distance + 2.f * ppCubeRadius;
				}

				fNear = std::max( 0.001f, fNear );
				lightProjection = PerspectiveMatrix( fFovy, fAspect, fNear, fFar );
			}
		}

		auto shadowViewProjMat = virtualCameraView * virtualCameraProjection;
		shadowViewProjMat *= lightView;
		shadowViewProjMat *= lightProjection;
		auto shadowMat = viewMatrix * shadowViewProjMat;

		SplitShadowProjectionMatrix( shadowInfo, view, shadowViewProjMat, shadowMat );
	}

	void BuildLSPSMProjectionMatrix( ShadowInfo& shadowInfo )
	{
		const LightSceneInfo* lightSceneInfo = shadowInfo.GetLightSceneInfo();
		LightProperty lightProperty = lightSceneInfo->Proxy()->GetLightProperty();

		const RenderView& view = *shadowInfo.View();
		Matrix viewMatrix = LookFromMatrix( view.m_viewOrigin,
			view.m_viewAxis[2],
			view.m_viewAxis[1] );

		float cosGamma = -lightProperty.m_direction | Vector( viewMatrix._13, viewMatrix._23, viewMatrix._33 );

		if ( abs( cosGamma ) >= 0.999f )
		{
			BuildOrthoShadowProjectionMatrix( shadowInfo );
		}
		else
		{
			RenderThreadFrameData<Vector> bodyB;
			bodyB.reserve( shadowInfo.ShadowCastersViewSpaceBounds().size() * 8 + 8 );

			for ( const BoxSphereBounds& bounds : shadowInfo.ShadowCastersViewSpaceBounds() )
			{
				Vector max = bounds.Origin() + bounds.HalfSize();
				Vector min = bounds.Origin() - bounds.HalfSize();

				for ( uint32 i = 0; i < 8; ++i )
				{
					bodyB.emplace_back( ( i & 1 ) ? min.x : max.x, ( i & 2 ) ? min.y : max.y, ( i & 4 ) ? min.z : max.z );
				}
			}

			Vector upVector = viewMatrix.TransformVector( -lightProperty.m_direction ).GetNormalized();

			Vector rightVector = ( upVector ^ Vector::ForwardVector ).GetNormalized();
			Vector lookVector = ( rightVector ^ upVector ).GetNormalized();

			Matrix lightSpaceBasis(
				rightVector.x, upVector.x, lookVector.x, 0.f,
				rightVector.y, upVector.y, lookVector.y, 0.f,
				rightVector.z, upVector.z, lookVector.z, 0.f,
				0.f, 0.f, 0.f, 1.f
			);

			lightSpaceBasis.TransformPosition( bodyB.data(), bodyB.data(), static_cast<uint32>( bodyB.size() ) );

			AxisAlignedBox lightSpaceBox( bodyB.data(), static_cast<uint32>( bodyB.size() ) );
			Vector lightSpaceOrigin = lightSpaceBox.Centroid();
			float sinGamma = sqrtf( 1.f - cosGamma * cosGamma );

			float receiversNear = -FLT_MAX;
			float receiversFar = FLT_MAX;
			for ( const BoxSphereBounds& bounds : shadowInfo.ShadowReceiversViewSpaceBounds() )
			{
				Vector max = bounds.Origin() + bounds.HalfSize();
				Vector min = bounds.Origin() - bounds.HalfSize();

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

			lightSpaceOrigin.z = lightSpaceBox.GetMin().z - nOpt;

			float maxX = -FLT_MAX, maxY = -FLT_MAX, maxZ = -FLT_MAX;

			Vector tmp;
			for ( const auto& point : bodyB )
			{
				tmp = point - lightSpaceOrigin;

				maxX = std::max( maxX, abs( tmp.x / tmp.z ) );
				maxY = std::max( maxY, abs( tmp.y / tmp.z ) );
				maxZ = std::max( maxZ, tmp.z );
			}

			Matrix translate = TranslationMatrix( -lightSpaceOrigin.x, -lightSpaceOrigin.y, -lightSpaceOrigin.z );
			Matrix perspective = PerspectiveMatrix( { 2.f * maxX * nOpt, 2.f * maxY * nOpt }, nOpt, maxZ );

			lightSpaceBasis *= translate;
			lightSpaceBasis *= perspective;

			Matrix permute(
				1.f, 0.f, 0.f, 0.f,
				0.f, 0.f, -1.f, 0.f,
				0.f, 1.f, 0.f, 0.f,
				0.f, -0.5f, 1.5f, 1.f
			);

			Matrix ortho = OrthoMatrix( { 2.f, 1.f }, 0.5f, 2.5f );

			lightSpaceBasis = lightSpaceBasis * permute;
			Matrix lightProjection = ortho;

			if ( false /*m_isUnitClipCube*/ && ( shadowInfo.ShadowReceiversViewSpaceBounds().size() > 0 ) )
			{
				RenderThreadFrameData<Vector> receiverPoints;
				receiverPoints.reserve( shadowInfo.ShadowReceiversViewSpaceBounds().size() * 8 );

				for ( const BoxSphereBounds& bounds : shadowInfo.ShadowReceiversViewSpaceBounds() )
				{
					Vector max = bounds.Origin() + bounds.HalfSize();
					Vector min = bounds.Origin() - bounds.HalfSize();

					for ( uint32 i = 0; i < 8; ++i )
					{
						receiverPoints.emplace_back( ( i & 1 ) ? min.x : max.x, ( i & 2 ) ? min.y : max.y, ( i & 4 ) ? min.z : max.z );
					}
				}

				lightSpaceBasis.TransformPosition( receiverPoints.data(), receiverPoints.data(), static_cast<uint32>( receiverPoints.size() ) );
				AxisAlignedBox receiverBox( receiverPoints.data(), static_cast<uint32>( receiverPoints.size() ) );

				float minX = std::min( -1.f, receiverBox.GetMin().x );
				float minY = std::min( -1.f, receiverBox.GetMin().y );
				maxX = std::max( 1.f, receiverBox.GetMax().x );
				maxY = std::max( 1.f, receiverBox.GetMax().y );

				float boxWidth = maxX - minX;
				float boxHeight = maxY - minY;

				if ( fpclassify( boxWidth ) != FP_ZERO && fpclassify( boxHeight ) != FP_ZERO )
				{
					float boxX = ( minX + maxX ) * 0.5f;
					float boxY = ( minY + maxY ) * 0.5f;

					Matrix clip(
						2.f / boxWidth, 0.f, 0.f, 0.f,
						0.f, 2.f / boxHeight, 0.f, 0.f,
						0.f, 0.f, 1.f, 0.f,
						-2.f * boxX / boxWidth, -2.f * boxY / boxHeight, 0.f, 1.f
					);

					lightProjection = lightProjection * clip;
				}
			}

			auto shadowViewProjMat = lightSpaceBasis * lightProjection;
			auto shadowMat = viewMatrix * shadowViewProjMat;

			SplitShadowProjectionMatrix( shadowInfo, view, shadowViewProjMat, shadowMat );
		}
	}

	void BuildPointShadowProjectionMatrix( ShadowInfo& shadowInfo )
	{
		LightSceneInfo* lightSceneInfo = shadowInfo.GetLightSceneInfo();
		
		float range = lightSceneInfo->Proxy()->GetRange();
		PerspectiveMatrix lightProjection( XMConvertToRadians( 90.f ), 1.f, 1, range );

		const Vector& position = lightSceneInfo->Proxy()->GetPosition();

		// +x
		shadowInfo.ShadowViewProjections()[0] = LookAtMatrix( position, position + Vector( 1, 0, 0 ), Vector( 0, 1, 0 ) ) * lightProjection;

		// -x
		shadowInfo.ShadowViewProjections()[1] = LookAtMatrix( position, position + Vector( -1, 0, 0 ), Vector( 0, 1, 0 ) ) * lightProjection;
		
		// +y
		shadowInfo.ShadowViewProjections()[2] = LookAtMatrix( position, position + Vector( 0, 1, 0 ), Vector( 0, 0, -1 ) ) * lightProjection;
		
		// -y
		shadowInfo.ShadowViewProjections()[3] = LookAtMatrix( position, position + Vector( 0, -1, 0 ), Vector( 0, 0, 1 ) ) * lightProjection;
		
		// +z
		shadowInfo.ShadowViewProjections()[4] = LookAtMatrix( position, position + Vector( 0, 0, 1 ), Vector( 0, 1, 0 ) ) * lightProjection;
		
		// -z
		shadowInfo.ShadowViewProjections()[5] = LookAtMatrix( position, position + Vector( 0, 0, -1 ), Vector( 0, 1, 0 ) ) * lightProjection;
	}

	void CalculateSplitPositions( ShadowInfo& shadowInfo, float casterFar, float lamda )
	{
		const RenderView& view = *shadowInfo.View();

		float invM = 1.f / CascadeShadowSetting::MAX_CASCADE_NUM;
		float farPlaneDistance = std::min( view.m_farPlaneDistance, casterFar );
		float fDivN = farPlaneDistance / view.m_nearPlaneDistance;
		float fSubN = farPlaneDistance - view.m_nearPlaneDistance;

		CascadeShadowSetting& cascadeSetting = shadowInfo.CascadeSetting();
		for ( uint32 i = 0; i <= CascadeShadowSetting::MAX_CASCADE_NUM; ++i )
		{
			float CiLog = view.m_nearPlaneDistance * std::powf( fDivN, invM * i );
			float CiUni = view.m_nearPlaneDistance * fSubN * i * invM;
			cascadeSetting.m_splitDistance[i] = lamda * CiLog + CiUni * ( 1.f - lamda );
		}

		cascadeSetting.m_splitDistance[0] = view.m_nearPlaneDistance;
		cascadeSetting.m_splitDistance[CascadeShadowSetting::MAX_CASCADE_NUM] = farPlaneDistance;
	}
}
