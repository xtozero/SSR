#pragma once

#include "GraphicsApiResource.h"
#include "IndexBuffer.h"
#include "LibraryTool/Common.h"
#include "MeshDescription.h"
#include "SizedTypes.h"
#include "VertexCollection.h"
#include "VertexLayout.h"

#include <filesystem>
#include <vector>

class Archive;

namespace rendercore
{
	struct MeshDescription;

	class StaticMeshSection final
	{
	public:
		friend Archive& operator<<( Archive& ar, StaticMeshSection& section );

		uint32 m_startLocation = 0;
		uint32 m_count = 0;
		uint32 m_materialIndex = 0;

		std::vector<Meshlet> m_meshlets;
		TypedBuffer<Meshlet> m_meshletBuffer;
	};

	class StaticMeshLODResource final
	{
	public:
		friend Archive& operator<<( Archive& ar, StaticMeshLODResource& lodResource );

		VertexCollection m_vertexCollection;

		bool m_isDWORD = false;
		std::vector<uint8> m_indexData;
		IndexBuffer m_ib;

		std::vector<StaticMeshSection> m_sections;

		std::vector<uint32> m_meshletVertices;
		TypedBuffer<uint32> m_meshletVertexBuffer;

		std::vector<uint32> m_meshletTriangles;
		TypedBuffer<uint32> m_meshletTriangleBuffer;
	};

	class StaticMeshRenderData final
	{
	public:
		RENDERCORE_DLL void AllocateLODResources( uint32 numLOD );
		RENDERCORE_DLL void Init();

		StaticMeshLODResource& LODResource( uint32 index ) { return m_lodResources[index]; };
		const StaticMeshLODResource& LODResource( uint32 index ) const { return m_lodResources[index]; };
		uint32 LODSize() const { return static_cast<uint32>( m_lodResources.size() ); }
		RefHandle<agl::BLAS> GetBLAS() const;

		void CreateRenderResource();

		bool Initialized() const { return m_initialized; }

		RENDERCORE_DLL StaticMeshRenderData() = default;

		friend Archive& operator<<( Archive& ar, StaticMeshRenderData& renderData );

	private:
		std::vector<StaticMeshLODResource> m_lodResources;
		RefHandle<agl::BLAS> m_blas;

		bool m_initialized = false;
	};
}
