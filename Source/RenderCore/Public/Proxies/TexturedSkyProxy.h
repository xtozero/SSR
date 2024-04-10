#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
#include "Math/Vector.h"
#include "RenderOption.h"
#include "Texture.h"
#include "VertexLayout.h"

#include <array>
#include <memory>

namespace logic
{
	class TexturedSkyComponent;
}

namespace rendercore
{
	class Material;
	class MaterialResource;
	class StaticMesh;
	class StaticMeshRenderData;

	class TexturedSkyProxy final
	{
	public:
		RENDERCORE_DLL TexturedSkyProxy( const logic::TexturedSkyComponent& component );

		void CreateRenderData();

		StaticMeshRenderData* GetRenderData();
		MaterialResource* GetMaterialResource();
		const DepthStencilOption& GetDepthStencilOption() const;
		const RasterizerOption& GetRasterizerOption() const;

		const std::array<Vector, 9>& IrradianceMapSH() const;
		agl::RefHandle<agl::Texture> IrradianceMap() const;

		agl::RefHandle<agl::Texture> PrefilteredColor() const;

	private:
		void PrefilterTexture();

		std::shared_ptr<const StaticMesh> m_pStaticMesh = nullptr;
		StaticMeshRenderData* m_pRenderData = nullptr;

		std::shared_ptr<const Material> m_pMaterial = nullptr;

		DepthStencilOption m_depthStencilOption;
		RasterizerOption m_rasterizerOption;

		agl::RefHandle<agl::Texture> m_irradianceMap;
		std::array<Vector, 9> m_irradianceMapSH;

		agl::RefHandle<agl::Texture> m_prefilteredSpecular;
	};
}
