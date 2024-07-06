#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
#include "IndexBuffer.h"
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

		uint32 m_startLocation;
		uint32 m_count;
		uint32 m_materialIndex;
	};

	class StaticMeshLODResource final
	{
	public:
		friend Archive& operator<<( Archive& ar, StaticMeshLODResource& lodResource );

		VertexCollection m_vertexCollection;
		bool m_isDWORD = false;
		std::vector<unsigned char> m_indexData;
		IndexBuffer m_ib;
		std::vector<StaticMeshSection> m_sections;
	};

	class StaticMeshRenderData final
	{
	public:
		RENDERCORE_DLL void AllocateLODResources( uint32 numLOD );
		RENDERCORE_DLL void Init();

		StaticMeshLODResource& LODResource( uint32 index ) { return m_lodResources[index]; };
		const StaticMeshLODResource& LODResource( uint32 index ) const { return m_lodResources[index]; };
		uint32 LODSize() const { return static_cast<uint32>( m_lodResources.size() ); }

		void CreateRenderResource();

		bool Initialized() const { return m_initialized; }

		RENDERCORE_DLL StaticMeshRenderData() = default;

		friend Archive& operator<<( Archive& ar, StaticMeshRenderData& renderData );

	private:
		std::vector<StaticMeshLODResource> m_lodResources;

		bool m_initialized = false;
	};
}
