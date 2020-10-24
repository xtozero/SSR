#include "stdafx.h"
#include "Model/StaticMesh.h"

#include "common.h"
#include "Mesh/StaticMeshResource.h"
#include "Model/CommonMeshDefine.h"
#include "MultiThread/EngineTaskScheduler.h"

 StaticMesh::StaticMesh( MeshDescription&& meshDescription, std::vector<Material>&& materials )
{
	 m_renderData = new StaticMeshRenderData( );
	 m_materials = std::move( materials );

	 m_renderData->AddLODResource( meshDescription, m_materials );

	 ENQUEUE_THREAD_TASK<ThreadType::RenderThread>( [this]( ){
		 m_renderData->InitRenderResource( );
	 } );
}

 StaticMesh::~StaticMesh( )
 {
	 // TODO: �����۾����� ���� ���� ���� ī��Ʈ �����ؾ� ��.
	 delete m_renderData;
 }
