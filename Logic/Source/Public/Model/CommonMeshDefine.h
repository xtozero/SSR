#pragma once

#include "Math/CXMFloat.h"

struct MeshVertex
{
	CXMFLOAT3 m_position;
	CXMFLOAT3 m_normal;
	CXMFLOAT3 m_color;
	CXMFLOAT2 m_texcoord;

	MeshVertex( ) :
		m_position( CXMFLOAT3( 0.f, 0.f, 0.f ) ),
		m_normal( CXMFLOAT3( 0.f, 0.f, 0.f ) ),
		m_color( CXMFLOAT3( 0.f, 0.f, 0.f ) ),
		m_texcoord( CXMFLOAT2( 0.f, 0.f ) ) {}
	
	explicit MeshVertex( const CXMFLOAT3& position ) :
		m_position( position ),
		m_normal( CXMFLOAT3( 0.f, 0.f, 0.f ) ),
		m_color( CXMFLOAT3( 0.f, 0.f, 0.f ) ),
		m_texcoord( CXMFLOAT2( 0.f, 0.f ) ) {}

	MeshVertex( const CXMFLOAT3& position, const CXMFLOAT3& normal ) :
		m_position( position ),
		m_normal( normal ),
		m_color( CXMFLOAT3( 0.f, 0.f, 0.f ) ),
		m_texcoord( CXMFLOAT2( 0.f, 0.f ) ) {}

	MeshVertex( const CXMFLOAT3& position, const CXMFLOAT3& normal, const CXMFLOAT2& texcoord ) :
		m_position( position ),
		m_normal( normal ),
		m_color( CXMFLOAT3( 0.f, 0.f, 0.f ) ),
		m_texcoord( texcoord ) {}

	MeshVertex( const CXMFLOAT3& position, const CXMFLOAT3& normal, const CXMFLOAT3& color ) :
		m_position( position ),
		m_normal( normal ),
		m_color( color ),
		m_texcoord( CXMFLOAT2( 0.f, 0.f ) ) {}

	MeshVertex( const CXMFLOAT3& position, const CXMFLOAT3& normal, const CXMFLOAT3& color, const CXMFLOAT2& texcoord ) :
		m_position( position ),
		m_normal( normal ),
		m_color( color ),
		m_texcoord( texcoord ) {}

	MeshVertex( const CXMFLOAT3& position, const CXMFLOAT2& texcoord ) :
		m_position( position ),
		m_normal( CXMFLOAT3( 0.f, 0.f, 0.f ) ),
		m_color( CXMFLOAT3( 0.f, 0.f, 0.f ) ),
		m_texcoord( texcoord ) {}
};

constexpr int VERTEX_STRIDE = sizeof( MeshVertex );