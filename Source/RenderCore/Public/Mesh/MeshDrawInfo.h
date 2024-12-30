#pragma once

#include "SizedTypes.h"

namespace rendercore
{
	class IndexBuffer;
	class MaterialResource;
	class Meshlet;
	class RenderOption;
	class VertexCollection;
	
	template <typename> class TypedBuffer;

	struct MeshDrawInfo
	{
		VertexCollection* m_vertexCollection = nullptr;
		IndexBuffer* m_indexBuffer = nullptr;

		TypedBuffer<Meshlet>* m_meshlet = nullptr;
		TypedBuffer<uint32>* m_meshletVertices = nullptr;
		TypedBuffer<uint32>* m_meshletTriangles = nullptr;

		MaterialResource* m_material = nullptr;
		const RenderOption* m_renderOption = nullptr;

		uint32 m_startLocation = 0;
		uint32 m_baseVertexLocation = 0;
		uint32 m_count = 0;
		uint32 m_numMeshlets = 0;

		uint32 m_lod = 0;
		uint32 m_sectionIndex = 0;
	};
}
