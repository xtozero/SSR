#pragma once;

#include "common.h"
#include "GraphicsApiResource.h"
#include "GraphicsResource/IndexBuffer.h"
#include "GraphicsResource/VertexBuffer.h"

#include <vector>

class Material;

struct MeshDescription;

struct StaticMeshVertex
{
	CXMFLOAT3 m_posision;
	CXMFLOAT3 m_normal;
	CXMFLOAT2 m_texcoord;
};

struct StaticMeshLODResource
{
	VertexBuffer<StaticMeshVertex> m_vb;
	IndexBuffer m_ib;
};

class StaticMeshVertexLayout : public VERTEX_LAYOUT
{
public:
	StaticMeshVertexLayout( const char* name, int index, RESOURCE_FORMAT format, int slot, bool isInstanceData, int instanceDataStep );

private:
	RE_HANDLE m_vertexLayout;
};

struct StaticMeshSection
{
	std::size_t m_startLocation;
	std::size_t m_count;
	std::size_t m_materialIndex;
};

class StaticMeshRenderData
{
public:
	RENDERCORE_DLL StaticMeshRenderData( ) = default;
	RENDERCORE_DLL void AddLODResource( const MeshDescription& meshDescription, const std::vector<Material>& materials );
	RENDERCORE_DLL void PostLODResourceSetup( );

private:
	std::vector<StaticMeshLODResource> m_lodResources;
	std::vector<StaticMeshVertexLayout> m_vertexLayouts;
	std::vector<StaticMeshSection> m_sections;
};