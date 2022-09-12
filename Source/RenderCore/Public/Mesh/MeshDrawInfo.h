#pragma once

#include "SizedTypes.h"

namespace rendercore
{
	class IndexBuffer;
	class MaterialResource;
	class RenderOption;
	class VertexCollection;

	struct MeshDrawInfo
	{
		VertexCollection* m_vertexCollection = nullptr;
		IndexBuffer* m_indexBuffer = nullptr;
		MaterialResource* m_material = nullptr;
		const RenderOption* m_renderOption = nullptr;

		uint32 m_startLocation = 0;
		uint32 m_count = 0;

		uint32 m_lod = 0;
		uint32 m_sectionIndex = 0;
	};
}
