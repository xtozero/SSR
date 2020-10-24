#pragma once;

#include "common.h"
#include "GraphicsApiResource.h"
#include "GraphicsResource/Buffer.h"
#include "IndexBuffer.h"
#include "Math/CXMFloat.h"

#include <vector>

class Material;

struct MeshDescription;

struct StaticMeshVertex
{
	CXMFLOAT3 m_posision;
	CXMFLOAT3 m_normal;
	CXMFLOAT2 m_texcoord;
};

struct StaticMeshSection
{
	std::size_t m_startLocation;
	std::size_t m_count;
	std::size_t m_materialIndex;
};

struct StaticMeshLODResource
{
	std::vector<StaticMeshVertex> m_vertexData;
	std::vector<std::size_t> m_indexData;
	TypeVertexBuffer<StaticMeshVertex> m_vb;
	RefHandle<IndexBuffer> m_ib;
	std::vector<StaticMeshSection> m_sections;
};

class StaticMeshVertexLayout
{
public:
	void Initialize( const StaticMeshLODResource* lodResource );

private:
	void AddLayout( const char* name, int index, RESOURCE_FORMAT format, int slot, bool isInstanceData, int instanceDataStep );

	std::vector<VERTEX_LAYOUT> m_vertexLayoutData;
	RE_HANDLE m_vertexLayout;
};

class StaticMeshRenderData
{
public:
	RENDERCORE_DLL StaticMeshRenderData( ) = default;
	RENDERCORE_DLL void AddLODResource( const MeshDescription& meshDescription, const std::vector<Material>& materials );
	RENDERCORE_DLL void InitRenderResource( );

private:
	std::vector<StaticMeshLODResource> m_lodResources;
	std::vector<StaticMeshVertexLayout> m_vertexLayouts;
};