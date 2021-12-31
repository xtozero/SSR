#pragma once

#include "Math/Vector.h"
#include "Math/Vector2.h"
#include "SizedTypes.h"

#include <cstddef>

struct MeshData
{
	void* m_pVertexData = nullptr;
	void* m_pIndexData = nullptr;

	uint32 m_vertices = 0;
	uint32 m_stride = 0;
	uint32 m_indices = 0;
};

struct MeshVertex
{
	Point m_position;
	Vector m_normal;
	Vector m_color;
	Point2 m_texcoord;

	MeshVertex( ) {}
	
	explicit MeshVertex( const Point& position ) :
		m_position( position ) {}

	MeshVertex( const Point& position, const Vector& normal ) :
		m_position( position ),
		m_normal( normal ) {}

	MeshVertex( const Point& position, const Vector& normal, const Point2& texcoord ) :
		m_position( position ),
		m_normal( normal ),
		m_texcoord( texcoord ) {}

	MeshVertex( const Point& position, const Vector& normal, const Vector& color ) :
		m_position( position ),
		m_normal( normal ),
		m_color( color ) {}

	MeshVertex( const Point& position, const Vector& normal, const Vector& color, const Point2& texcoord ) :
		m_position( position ),
		m_normal( normal ),
		m_color( color ),
		m_texcoord( texcoord ) {}

	MeshVertex( const Point& position, const Vector2& texcoord ) :
		m_position( position ),
		m_texcoord( texcoord ) {}
};

constexpr size_t VERTEX_STRIDE = sizeof( MeshVertex );