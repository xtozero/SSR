#pragma once;

#include "common.h"
#include "GraphicsApiResource.h"
#include "IndexBuffer.h"
#include "Math/CXMFloat.h"
#include "SizedTypes.h"
#include "TypedBuffer.h"
#include "VertexLayout.h"

#include <filesystem>
#include <vector>

class Archive;

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

	uint32 m_startLocation;
	uint32 m_count;
	uint32 m_materialIndex;
};

class StaticMeshLODResource
{
public:
	void Serialize( Archive& ar );

	std::vector<StaticMeshVertex> m_vertexData;
	bool m_isDWORD = false;
	std::vector<unsigned char> m_indexData;
	TypedVertexBuffer<StaticMeshVertex> m_vb;
	IndexBuffer m_ib;
	std::vector<StaticMeshSection> m_sections;
};

class StaticMeshVertexLayout
{
public:
	void Initialize( const StaticMeshLODResource* lodResource );

	const VertexLayoutDesc& Desc( ) const { return m_vertexLayoutDesc; }

private:
	VertexLayoutDesc m_vertexLayoutDesc;
};

class StaticMeshRenderData
{
public:
	RENDERCORE_DLL void AllocateLODResources( uint32 numLOD );
	RENDERCORE_DLL void Init( );
	RENDERCORE_DLL void Serialize( Archive& ar );

	StaticMeshLODResource& LODResource( uint32 index ) { return m_lodResources[index]; };
	const StaticMeshLODResource& LODResource( uint32 index ) const { return m_lodResources[index]; };
	uint32 LODSize( ) const { return static_cast<uint32>( m_lodResources.size( ) ); }
	const StaticMeshVertexLayout& VertexLayout( uint32 index ) const { return m_vertexLayouts[index]; }

	void CreateRenderResource( );

	bool Initialized( ) const { return m_initialized; }

	RENDERCORE_DLL StaticMeshRenderData( ) = default;

private:
	std::vector<StaticMeshLODResource> m_lodResources;
	std::vector<StaticMeshVertexLayout> m_vertexLayouts;
	
	bool m_initialized = false;
};