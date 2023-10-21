#include "stdafx.h"
#include "TexturedSkyProxy.h"

#include "Components/TexturedSkyComponent.h"
#include "IrradianceMapRendering.h"
#include "Material.h"
#include "MaterialResource.h"
#include "Mesh/StaticMesh.h"
#include "Mesh/StaticMeshResource.h"

namespace rendercore
{
	TexturedSkyProxy::TexturedSkyProxy( const logic::TexturedSkyComponent& component ) : m_pStaticMesh( component.GetStaticMesh() ), m_pRenderData( m_pStaticMesh->RenderData() ), m_pMaterial( component.GetMaterial() )
	{
		m_depthStencilOption.m_depth.m_enable = false;
		m_depthStencilOption.m_depth.m_writeDepth = false;
		m_rasterizerOption.m_scissorEnable = true;
	}

	void TexturedSkyProxy::CreateRenderData()
	{
		assert( IsInRenderThread() );
		m_pRenderData->CreateRenderResource();
		PrefilterTexture();
	}

	StaticMeshRenderData* TexturedSkyProxy::GetRenderData()
	{
		return m_pRenderData;
	}

	MaterialResource* TexturedSkyProxy::GetMaterialResource()
	{
		return m_pMaterial->GetMaterialResource();
	}

	const DepthStencilOption& TexturedSkyProxy::GetDepthStencilOption() const
	{
		return m_depthStencilOption;
	}

	const RasterizerOption& TexturedSkyProxy::GetRasterizerOption() const
	{
		return m_rasterizerOption;
	}

	void TexturedSkyProxy::PrefilterTexture()
	{
		MaterialResource* materialResource = GetMaterialResource();
		if ( materialResource == nullptr )
		{
			return;
		}

		const std::shared_ptr<Material> material = materialResource->GetMaterial();
		if ( material == nullptr )
		{
			return;
		}

		rendercore::Texture* skyTexture = material->AsTexture( "SkyTexture" );
		if ( skyTexture == nullptr )
		{
			return;
		}

		m_irradianceMap = GenerateIrradianceMap( skyTexture->Resource() );
		m_irradianceMapSH = GenerateIrradianceMapSH( skyTexture->Resource() );
	}
}
