#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
#include "NameTypes.h"
#include "SizedTypes.h"
#include "VertexBuffer.h"
#include "VertexLayout.h"

#include <optional>
#include <vector>

class Archive;

namespace rendercore
{
	class VertexBufferBundle;
	struct MeshDescription;

	class VertexStream
	{
	public:
		uint8* Data()
		{
			return m_data.data();
		}

		const uint8* Data() const
		{
			return m_data.data();
		}

		const Name& GetName() const
		{
			return m_name;
		}

		uint32 Stride() const
		{
			return m_stride;
		}

		uint32 Count() const
		{
			return m_count;
		}

		bool IsDynamic() const
		{
			return m_isDynamic;
		}

		VertexStream( const char* name, agl::ResourceFormat format, uint32 count, bool isDynamic = false );
		VertexStream() = default;

		friend Archive& operator<<( Archive& ar, VertexStream& stream );

	private:
		Name m_name;
		agl::ResourceFormat m_format = agl::ResourceFormat::Unknown;
		uint32 m_stride = 0;
		uint32 m_count = 0;
		bool m_isDynamic = false;

		std::vector<uint8> m_data;
	};

	class VertexStreamLayout : public VertexLayoutDesc
	{
	public:
		void AddLayout( const char* name, uint32 index, agl::ResourceFormat format, uint32 slot, bool isInstanceData, uint32 instanceDataStep, int32 streamIndex );

		int32 StreamIndex( uint32 index ) const
		{
			return m_streamIndices[index];
		}

		friend Archive& operator<<( Archive& ar, VertexStreamLayout& layout );

	private:
		int32 m_streamIndices[agl::MAX_VERTEX_LAYOUT_SIZE] = {};
	};

	enum class VertexStreamLayoutType : uint8
	{
		Default = 0,
		PositionNormal,
		PositionOnly,
	};

	class VertexCollection
	{
	public:
		void InitResource();

		uint32 AddStream( const VertexStream& stream );
		uint32 AddStream( VertexStream&& stream );

		[[nodiscard]] VertexBuffer* GetVertexBuffer( const Name& name );

		void InitLayout( const agl::VertexLayoutTrait* traits, uint32 count, VertexStreamLayoutType layoutType );

		const VertexStreamLayout& VertexLayout( VertexStreamLayoutType layoutType ) const;

		void Bind( VertexBufferBundle& bundle, VertexStreamLayoutType layoutType ) const;

		friend Archive& operator<<( Archive& ar, VertexCollection& collection );

	private:
		std::optional<uint32> FindStreamIndex( const Name& name ) const;
		VertexStreamLayout SetupVertexLayout( const agl::VertexLayoutTrait* trait, uint32 count );

		std::vector<VertexStream> m_streams;
		mutable std::vector<VertexBuffer> m_vbs;

		VertexStreamLayout m_positionLayout;
		VertexStreamLayout m_positionNormalLayout;
		VertexStreamLayout m_defaultLayout;
	};

	VertexCollection BuildFromMeshDescription( const MeshDescription& desc );
}
