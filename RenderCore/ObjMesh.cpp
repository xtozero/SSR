#include "stdafx.h"
#include "ObjMesh.h"

#include "IRenderer.h"
#include "Material.h"
#include "../Shared/Util.h"

#include <d3dX9math.h>

extern IRenderer* g_pRenderer;

bool CObjMesh::Load( D3D_PRIMITIVE_TOPOLOGY topology )
{
	bool loadSuccess = BaseMesh::Load( topology );

	FOR_EACH_VEC( m_mtlGroup, i )
	{
		if ( i->m_textureName.size( ) > 0 )
		{
			auto texture = g_pRenderer->GetTextureFromFile( i->m_textureName );

			if ( texture.get( ) )
			{
				i->m_pTexture = texture;
			}
			else
			{
				return false;
			}
		}
	}

	return loadSuccess;
}

void CObjMesh::Draw( ID3D11DeviceContext* pDeviceContext )
{
	if ( !pDeviceContext )
	{
		return;
	}

	if ( !m_pMaterial )
	{
		return;
	}

	if ( !m_pVertexBuffer )
	{
		return;
	}

	m_pMaterial->SetShader( pDeviceContext );
	m_pMaterial->SetPrimitiveTopology( pDeviceContext, m_primitiveTopology );
	m_pVertexBuffer->SetIABuffer( pDeviceContext, &m_nOffset );
	if ( m_pIndexBuffer )
	{
		m_pIndexBuffer->SetIABuffer( pDeviceContext, &m_nIndexOffset );

		if ( m_mtlGroup.size( ) == 0 )
		{
			m_pMaterial->DrawIndexed( pDeviceContext, m_nIndices, m_nIndexOffset, m_nOffset );
		}
		else
		{
			FOR_EACH_VEC( m_mtlGroup, i )
			{
				m_pMaterial->SetTexture( pDeviceContext, SHADER_TYPE::PS, 0, i->m_pTexture );
				m_pMaterial->DrawIndexed( pDeviceContext, i->m_indexCount, i->m_indexOffset, m_nOffset );
			}
		}
	}
	else
	{
		m_pMaterial->Draw( pDeviceContext, m_nVertices, m_nOffset );
	}
}

void CObjMesh::AddMaterialGroup( const ObjMaterialTrait& trait )
{
	m_mtlGroup.emplace_back( trait );
}

CObjMesh::CObjMesh( )
{
}


CObjMesh::~CObjMesh( )
{
}
