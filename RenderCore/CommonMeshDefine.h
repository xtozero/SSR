#pragma once

#include <d3dX9math.h>

struct MeshVertex
{
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_normal;
	D3DXVECTOR3 m_color;
	D3DXVECTOR2 m_texcoord;

	MeshVertex( ) :
		m_position( D3DXVECTOR3( 0.f, 0.f, 0.f ) ),
		m_normal( D3DXVECTOR3( 0.f, 0.f, 0.f ) ),
		m_color( D3DXVECTOR3( 0.f, 0.f, 0.f ) ),
		m_texcoord( D3DXVECTOR2( 0.f, 0.f ) ) {}
	
	MeshVertex( const D3DXVECTOR3& position ) :
		m_position( position ),
		m_normal( D3DXVECTOR3( 0.f, 0.f, 0.f ) ),
		m_color( D3DXVECTOR3( 0.f, 0.f, 0.f ) ),
		m_texcoord( D3DXVECTOR2( 0.f, 0.f ) ) {}

	MeshVertex( const D3DXVECTOR3& position, const D3DXVECTOR3& normal ) :
		m_position( position ),
		m_normal( normal ),
		m_color( D3DXVECTOR3( 0.f, 0.f, 0.f ) ),
		m_texcoord( D3DXVECTOR2( 0.f, 0.f ) ) {}

	MeshVertex( const D3DXVECTOR3& position, const D3DXVECTOR3& normal, const D3DXVECTOR3& color ) :
		m_position( position ),
		m_normal( normal ),
		m_color( color ),
		m_texcoord( D3DXVECTOR2( 0.f, 0.f ) ) {}

	MeshVertex( const D3DXVECTOR3& position, const D3DXVECTOR3& normal, const D3DXVECTOR3& color, const D3DXVECTOR2& texcoord ) :
		m_position( position ),
		m_normal( normal ),
		m_color( color ),
		m_texcoord( texcoord ) {}
};

const int VERTEX_STRIDE = sizeof( MeshVertex );