#pragma once

#include "GraphicsApiResource.h"
#include "Math/Vector4.h"
#include "PassProcessor.h"
#include "Texture.h"
#include "TypedBuffer.h"

class Scene;

struct PrimitiveSubMesh;

namespace rendercore
{
	struct SkyAtmosphereRenderParameter final
	{
		Vector4 m_cameraPos;
		Vector4 m_sunDir;
		float m_exposure;
		float padding[3];
	};

	class SkyAtmosphereRenderSceneInfo final
	{
	public:
		agl::RefHandle<agl::Texture>& GetTransmittanceLutTexture()
		{
			return m_transmittanceLutTexture;
		}

		agl::RefHandle<agl::Texture>& GetIrradianceLutTexture()
		{
			return m_irradianceLutTexture;
		}

		agl::RefHandle<agl::Texture>& GetInscatterLutTexture()
		{
			return m_inscatterLutTexture;
		}

		TypedConstatBuffer<SkyAtmosphereRenderParameter>& GetSkyAtmosphereRenderParameter()
		{
			return m_skyAtmosphereRenderParameter;
		}

		bool& RebuildLookUpTables()
		{
			return m_rebuildLuts;
		}

	private:
		agl::RefHandle<agl::Texture> m_transmittanceLutTexture;
		agl::RefHandle<agl::Texture> m_irradianceLutTexture;
		agl::RefHandle<agl::Texture> m_inscatterLutTexture;
		TypedConstatBuffer<SkyAtmosphereRenderParameter> m_skyAtmosphereRenderParameter;
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
