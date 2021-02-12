#pragma once;

#include "common.h"
#include "GraphicsApiResource.h"
#include "IndexBuffer.h"
#include "Math/CXMFloat.h"
#include "TypedBuffer.h"
#include "VertexInputLayout.h"

#include <filesystem>
#include <vector>

class Archive;
class Material;

struct MeshDescription;

class StaticMeshVertex
{
public:
	void Serialize( Archive& ar );

	CXMFLOAT3 m_position;
	CXMFLOAT3 m_normal;
	CXMFLOAT2 m_texcoord;
};

class StaticMeshSection
{
public:
	void Serialize( Archive& ar );

	std::size_t m_startLocation;
	std::size_t m_count;
	std::size_t m_materialIndex;
};

class StaticMeshLODResource
{
public:
	void Serialize( Archive& ar );

	std::vector<StaticMeshVertex> m_vertexData;
	std::vector<std::size_t> m_indexData;
	TypeVertexBuffer<StaticMeshVertex> m_vb;
	IndexBuffer m_ib;
	std::vector<StaticMeshSection> m_sections;
};

class StaticMeshVertexLayout
{
public:
	void Initialize( const StaticMeshLODResource* lodResource );

private:
	VertexLayoutDesc m_vertexLayoutDesc;
	RE_HANDLE m_vertexLayout;
};

class StaticMeshRenderData
{
public:
	RENDERCORE_DLL void AllocateLODResources( std::size_t numLOD );
	RENDERCORE_DLL void InitRenderResource( );
	RENDERCORE_DLL void Serialize( Archive& ar );

	StaticMeshLODResource& LODResource( int index ) { return m_lodResources[index]; };

	RENDERCORE_DLL StaticMeshRenderData( ) = default;

private:
	std::vector<StaticMeshLODResource> m_lodResources;
	std::vector<StaticMeshVertexLayout> m_vertexLayouts;
};