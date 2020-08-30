#include "stdafx.h"
#include "Model/StaticMesh.h"

#include "common.h"
#include "Mesh/StaticMeshResource.h"
#include "Model/CommonMeshDefine.h"

 StaticMesh::StaticMesh( MeshDescription&& meshDescription, std::vector<Material>&& materials )
{
	 m_renderData = new StaticMeshRenderData( );
	 m_materials = std::move( materials );

	 m_renderData->AddLODResource( meshDescription, m_materials );
	 m_renderData->PostLODResourceSetup( );
}
