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
	 // TODO: 추후작업으로 삭제 말고 레퍼 카운트 감소해야 함.
	 delete m_renderData;
 }
