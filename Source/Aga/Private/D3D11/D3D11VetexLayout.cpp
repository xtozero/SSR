#include "stdafx.h"
#include "D3D11VetexLayout.h"

#include "D3D11Api.h"
#include "D3D11FlagConvertor.h"
#include "D3D11Shaders.h"

namespace
{
	void ConvertVertexLayoutToInputLayout( D3D11_INPUT_ELEMENT_DESC* iL, const VERTEX_LAYOUT* vL, int numLayout )
	{
		for ( int i = 0; i < numLayout; ++i )
		{
			iL[i].SemanticName = vL[i].m_name.c_str( );
			iL[i].SemanticIndex = vL[i].m_index;
			iL[i].Format = ConvertFormatToDxgiFormat( vL[i].m_format );
			iL[i].InputSlot = vL[i].m_slot;
			iL[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			iL[i].InputSlotClass = vL[i].m_isInstanceData ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
			iL[i].InstanceDataStepRate = vL[i].m_instanceDataStep;
		}
	}
}

void CD3D11VertexLayout::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateInputLayout( m_imputDesc, m_layoutSize, m_vs->ByteCode( ), m_vs->ByteCodeSize( ), m_pResource.GetAddressOf( ) ) );
	assert( result );
}

CD3D11VertexLayout::CD3D11VertexLayout( const RefHandle<CD3D11VertexShader>& vs, const VERTEX_LAYOUT* layout, int layoutSize ) : m_vs( vs ), m_layoutSize( layoutSize )
{
	m_imputDesc = new D3D11_INPUT_ELEMENT_DESC[m_layoutSize];
	ConvertVertexLayoutToInputLayout( m_imputDesc, layout, m_layoutSize );
}

CD3D11VertexLayout::~CD3D11VertexLayout( )
{
	delete[] m_imputDesc;
}
