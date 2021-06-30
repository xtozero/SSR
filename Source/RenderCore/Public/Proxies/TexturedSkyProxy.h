#pragma once

#include "common.h"
#include "RenderOption.h"
#include "VertexLayout.h"

#include <memory>

class Material;
class MaterialResource;
class StaticMesh;
class StaticMeshRenderData;
class TexturedSkyComponent;

class TexturedSkyProxy
{
public:
	RENDERCORE_DLL TexturedSkyProxy( const TexturedSkyComponent& component );

	void CreateRenderData( );

	StaticMeshRenderData* GetRenderData( );
	MaterialResource* GetMaterialResource( );
	const DepthStencilOption& GetDepthStencilOption( ) const;
	const RasterizerOption& GetRasterizerOption( ) const;

private:
	std::shared_ptr<const StaticMesh> m_pStaticMesh = nullptr;
	StaticMeshRenderData* m_pRenderData = nullptr;

	std::shared_ptr<const Material> m_pMaterial = nullptr;

	DepthStencilOption m_depthStencilOption;
	RasterizerOption m_rasterizerOption;
};