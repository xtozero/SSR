#pragma once

#include "GraphicsApiResource.h"
#include "Math/Vector4.h"
#include "PassProcessor.h"
#include "Texture.h"

class Scene;

struct PrimitiveSubMesh;

namespace rendercore
{
	BEGIN_SHADER_ARGUMENTS_STRUCT( SkyAtmosphereRenderParameters )
		DECLARE_VALUE( Vector4, CameraPos )
		DECLARE_VALUE( Vector4, SunDir )
		DECLARE_VALUE( float, Exposure )
	END_SHADER_ARGUMENTS_STRUCT()

	class SkyAtmosphereRenderSceneInfo final
	{
	public:
		RefHandle<agl::Texture>& GetTransmittanceLutTexture()
		{
			return m_transmittanceLutTexture;
		}

		RefHandle<agl::Texture>& GetIrradianceLutTexture()
		{
			return m_irradianceLutTexture;
		}

		RefHandle<agl::Texture>& GetInscatterLutTexture()
		{
			return m_inscatterLutTexture;
		}

		RefHandle<ShaderArguments>& GetShaderArguments()
		{
			return m_shaderArguments;
		}

		bool& RebuildLookUpTables()
		{
			return m_rebuildLuts;
		}

	private:
		RefHandle<agl::Texture> m_transmittanceLutTexture;
		RefHandle<agl::Texture> m_irradianceLutTexture;
		RefHandle<agl::Texture> m_inscatterLutTexture;
		RefHandle<ShaderArguments> m_shaderArguments;
		bool m_rebuildLuts = true;
	};

	class SkyAtmosphereDrawPassProcessor final : public IPassProcessor
	{
	public:
		virtual std::optional<DrawSnapshot> Process( const PrimitiveSubMesh& subMesh ) override;
	};

	void InitAtmosphereForScene( Scene& scene );
	void RenderAtmosphereLookUpTables( Scene& scene );
}
