#include "stdafx.h"
#include "TexturedSkyProxy.h"

#include "Components/TexturedSkyComponent.h"
#include "Material.h"
#include "Mesh/StaticMesh.h"
#include "Mesh/StaticMeshResource.h"

namespace rendercore
{
	TexturedSkyProxy::TexturedSkyProxy( const TexturedSkyComponent& component ) : m_pStaticMesh( component.GetStaticMesh() ), m_pRenderData( m_pStaticMesh->RenderData() ), m_pMaterial( component.GetMaterial() )
	{
		m_depthStencilOption.m_depth.m_enable = false;
		m_depthStencilOption.m_depth.m_writeDepth = false;
		m_rasterizerOption.m_scissorEnable = true;
	}

	void TexturedSkyProxy::CreateRenderData()
	{
		assert( IsInRenderThread() );
		m_pRenderData->CreateRenderResource();
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
}
