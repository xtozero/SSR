#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
#include "IndexBuffer.h"
#include "Math/CXMFloat.h"
#include "SizedTypes.h"
#include "TypedBuffer.h"
#include "VertexCollection.h"
#include "VertexLayout.h"

#include <filesystem>
#include <vector>

class Archive;

struct MeshDescription;

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

	VertexCollection m_vertexCollection;
	bool m_isDWORD = false;
	std::vector<unsigned char> m_indexData;
	IndexBuffer m_ib;
	std::vector<StaticMeshSection> m_sections;
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

	void CreateRenderResource( );

	bool Initialized( ) const { return m_initialized; }

	RENDERCORE_DLL StaticMeshRenderData( ) = default;

private:
	std::vector<StaticMeshLODResource> m_lodResources;

	bool m_initialized = false;
};