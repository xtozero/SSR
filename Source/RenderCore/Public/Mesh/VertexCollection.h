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

	VertexStream( const char* name, RESOURCE_FORMAT format, uint32 count );
	VertexStream() = default;

	friend Archive& operator<<( Archive& ar, VertexStream& stream );

private:
	Name m_name;
	RESOURCE_FORMAT m_format = RESOURCE_FORMAT::UNKNOWN;
	uint32 m_stride = 0;
	uint32 m_count = 0;

	std::vector<uint8> m_data;
};

class VertexStreamLayout : public VertexLayoutDesc
{
public:
	void AddLayout( const char* name, uint32 index, RESOURCE_FORMAT format, uint32 slot, bool isInstanceData, uint32 instanceDataStep, int32 streamIndex );

	int32 StreamIndex( uint32 index ) const
	{
		return m_streamIndices[index];
	}

	friend Archive& operator<<( Archive& ar, VertexStreamLayout& layout );

private:
	int32 m_streamIndices[MAX_VERTEX_LAYOUT_SIZE] = {};
};

enum class VertexStreamLayoutType
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

	void InitLayout( const VERTEX_LAYOUT_TRAIT* traits, uint32 count, VertexStreamLayoutType layoutType );

	const VertexStreamLayout& VertexLayout( VertexStreamLayoutType layoutType ) const;

	void Bind( VertexBufferBundle& bundle, VertexStreamLayoutType layoutType ) const;

	friend Archive& operator<<( Archive& ar, VertexCollection& collection );

private:
	std::optional<uint32> FindStream( const Name& name ) const;
	VertexStreamLayout SetupVertexLayout( const VERTEX_LAYOUT_TRAIT* trait, uint32 count );

	std::vector<VertexStream> m_streams;
	mutable std::vector<VertexBuffer> m_vbs;

	VertexStreamLayout m_positionLayout;
	VertexStreamLayout m_positionNormalLayout;
	VertexStreamLayout m_defaultLayout;
};

VertexCollection BuildFromMeshDescription( const MeshDescription& desc );
