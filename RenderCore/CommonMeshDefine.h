#pragma once

#include <d3dx9math.h>

struct MeshVertex
{
	D3DXVECTOR3 m_position;
	D3DXVECTOR3 m_normal;
	D3DXVECTOR3 m_color;
	D3DXVECTOR2 m_texcoord;
};

const int VERTEX_STRIDE = sizeof( MeshVertex );