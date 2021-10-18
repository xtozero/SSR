#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
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
	void Serialize( Archive& ar );

	uint8* Data( )
	{
		return m_data.data( );
	}

	const uint8* Data( ) const
	{
		return m_data.data( );
	}

	const std::string& Name( ) const
	{
		return m_name;
	}

	uint32 Stride( ) const
	{
		return m_stride;
	}

	uint32 Count( ) const
	{
		return m_count;
	}

	VertexStream( const char* name, RESOURCE_FORMAT format, uint32 count );
	VertexStream( ) = default;

private:
	std::string m_name;
	RESOURCE_FORMAT m_format = RESOURCE_FORMAT::UNKNOWN;
	uint32 m_stride = 0;
	uint32 m_count = 0;

	std::vector<uint8> m_data;
};

class VertexStreamLayout : public VertexLayoutDesc
{
public:
	virtual void Serialize( Archive& ar ) override;

	void AddLayout( const char* name, uint32 index, RESOURCE_FORMAT format, uint32 slot, bool isInstanceData, uint32 instanceDataStep, int32 streamIndex );

	int32 StreamIndex( uint32 index ) const
	{
		return m_streamIndices[index];
	}

private:
	int32 m_streamIndices[MAX_VERTEX_LAYOUT_SIZE] = {};
};

enum class VertexStreamLayoutType
{
	Default = 0,
	PositionOnly,
};

class VertexCollection
{
public:
	void Serialize( Archive& ar );

	void InitResource( );

	uint32 AddStream( const VertexStream& stream );
	uint32 AddStream( VertexStream&& stream );

	void InitLayout( const VERTEX_LAYOUT_TRAIT* traits, uint32 count, VertexStreamLayoutType layoutType );

	const VertexStreamLayout& VertexLayout( VertexStreamLayoutType layoutType ) const;

	void Bind( VertexBufferBundle& bundle, VertexStreamLayoutType layoutType ) const;

private:
	std::optional<uint32> FindStream( const std::string& name ) const;
	VertexStreamLayout SetupVertexLayout( const VERTEX_LAYOUT_TRAIT* trait, uint32 count );

	std::vector<VertexStream> m_streams;
	mutable std::vector<VertexBuffer> m_vbs;

	VertexStreamLayout m_defaultLayout;
	VertexStreamLayout m_positionLayout;
};

VertexCollection BuildFromMeshDescription( const MeshDescription& desc );
Archive& operator<<( Archive& ar, VertexCollection& vertexCollection );