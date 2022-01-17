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
	struct SkyAtmosphereRenderParameter
	{
		Vector4 m_cameraPos;
		Vector4 m_sunDir;
		float m_exposure;
		float m_padding[3];
	};

	class SkyAtmosphereRenderSceneInfo
	{
	public:
		RefHandle<aga::Texture>& GetTransmittanceLutTexture()
		{
			return m_transmittanceLutTexture;
		}

		RefHandle<aga::Texture>& GetIrradianceLutTexture()
		{
			return m_irradianceLutTexture;
		}

		RefHandle<aga::Texture>& GetInscatterLutTexture()
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
		RefHandle<aga::Texture> m_transmittanceLutTexture;
		RefHandle<aga::Texture> m_irradianceLutTexture;
		RefHandle<aga::Texture> m_inscatterLutTexture;
		TypedConstatBuffer<SkyAtmosphereRenderParameter> m_skyAtmosphereRenderParameter;
		bool m_rebuildLuts = true;
	};

	class SkyAtmosphereDrawPassProcessor : public IPassProcessor
	{
	public:
		virtual std::optional<DrawSnapshot> Process( const PrimitiveSubMesh& subMesh ) override;
	};

	void InitAtmosphereForScene( Scene& scene );
	void RenderAtmosphereLookUpTables( Scene& scene );
}