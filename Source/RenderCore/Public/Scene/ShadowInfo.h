#pragma once

#include "DrawSnapshot.h"
#include "GraphicsApiResource.h"
#include "InlineMemoryAllocator.h"
#include "Math/Matrix.h"
#include "Math/Vector4.h"
#include "Physics/BoxSphereBounds.h"
#include "ShaderArguments.h"
#include "SizedTypes.h"
#include "Texture.h"

namespace rendercore
{
	class LightSceneInfo;
	class PrimitiveSceneInfo;
	class RenderingShaderResource;
	class SceneRenderer;

	enum class LightType : uint8;

	struct RenderView;

	struct CascadeShadowSetting final
	{
		static constexpr uint32 MAX_CASCADE_NUM = 2;
		float m_splitDistance[MAX_CASCADE_NUM + 1] = {};
	};

	BEGIN_SHADER_ARGUMENTS_STRUCT( ShadowDepthPassParameters )
		DECLARE_VALUE( Vector4, LightPosOrDir )
		DECLARE_VALUE( float, SlopeBiasScale )
		DECLARE_VALUE( float, ConstantBias )
		DECLARE_VALUE( uint32, LightIndex )
		DECLARE_ARRAY_VALUE( Vector4, CascadeFar, CascadeShadowSetting::MAX_CASCADE_NUM )
		DECLARE_ARRAY_VALUE( Matrix, ShadowViewProjection, 6 )
	END_SHADER_ARGUMENTS_STRUCT()

	BEGIN_SHADER_ARGUMENTS_STRUCT( ESMsParameters )
		DECLARE_VALUE( float, EsmsParameterC )
	END_SHADER_ARGUMENTS_STRUCT()

	struct ShadowMapRenderTarget final
	{
		using ShadowMapList = std::vector<RefHandle<agl::Texture>, InlineAllocator<RefHandle<agl::Texture>, 1>>;

		ShadowMapList m_shadowMaps;
		RefHandle<agl::Texture> m_shadowMapDepth;
	};

	class ShadowInfo final
	{
	public:
		bool HasCasterPrimitives() const
		{
			return m_shadowCasters.size() > 0;
		}

		const ShadowMapRenderTarget& ShadowMap() const
		{
			return m_shadowMap;
		}

		ShadowMapRenderTarget& ShadowMap()
		{
			return m_shadowMap;
		}

		const LightSceneInfo* GetLightSceneInfo() const
		{
			return m_lightSceneInfo;
		}

		LightSceneInfo* GetLightSceneInfo()
		{
			return m_lightSceneInfo;
		}

		const RenderView* View() const
		{
			return m_view;
		}

		std::pair<uint32, uint32> ShadowMapSize() const
		{
			return { m_shadowMapWidth, m_shadowMapHeight };
		}

		const RenderThreadFrameData<BoxSphereBounds>& ShadowCastersViewSpaceBounds() const
		{
			return m_shadowCastersViewSpaceBounds;
		}

		const RenderThreadFrameData<BoxSphereBounds>& ShadowReceiversViewSpaceBounds() const
		{
			return m_shadowReceiversViewSpaceBounds;
		}

		float SubjectNear() const
		{
			return m_subjectNear;
		}

		float SubjectFar() const
		{
			return m_subjectFar;
		}

		Matrix( &ShadowViewProjections() )[6]
		{
			return m_shadowViewProjections;
		}

		CascadeShadowSetting& CascadeSetting()
		{
			return m_cacadeSetting;
		}

		ShaderArguments& GetShadowShaderArguments()
		{
			return *m_shadowShaderArguments.Get();
		}

		ShaderArguments& GetESMsShaderArguments()
		{
			return *m_esmsShaderArguments.Get();
		}

		LightType GetLightType() const;

		void AddCasterPrimitive( PrimitiveSceneInfo* primitiveSceneInfo );
		void AddCasterPrimitive( PrimitiveSceneInfo* primitiveSceneInfo, const BoxSphereBounds& viewspaceBounds );
		void AddReceiverPrimitive( PrimitiveSceneInfo* primitiveSceneInfo, const BoxSphereBounds& viewspaceBounds );

		void SetupShadowConstantBuffer();
		void RenderDepth( SceneRenderer& renderer, RenderingShaderResource& resources );

		ShadowInfo( LightSceneInfo* lightSceneInfo, const RenderView& view );

	private:
		void AddCachedDrawSnapshotForPass( PrimitiveSceneInfo& primitiveSceneInfo );
		void UpdateSubjectNearAndFar( const BoxSphereBounds& viewspaceBounds );
		RenderPass GetShadowDepthRenderPass() const;

		LightSceneInfo* m_lightSceneInfo = nullptr;
		const RenderView* m_view = nullptr;
		ShadowMapRenderTarget m_shadowMap;
		uint32 m_shadowMapWidth = 0;
		uint32 m_shadowMapHeight = 0;

		RenderThreadFrameData<PrimitiveSceneInfo*> m_shadowCasters;
		RenderThreadFrameData<PrimitiveSceneInfo*> m_shadowReceivers;

		RenderThreadFrameData<BoxSphereBounds> m_shadowCastersViewSpaceBounds;
		RenderThreadFrameData<BoxSphereBounds> m_shadowReceiversViewSpaceBounds;

		float m_subjectNear = std::numeric_limits<float>::max();
		float m_subjectFar = std::numeric_limits<float>::min();

		RenderThreadFrameData<VisibleDrawSnapshot> m_snapshots;

		Matrix m_shadowViewProjections[6];

		CascadeShadowSetting m_cacadeSetting;

		RefHandle<ShaderArguments> m_shadowShaderArguments;
		RefHandle<ShaderArguments> m_esmsShaderArguments;
	};
}
