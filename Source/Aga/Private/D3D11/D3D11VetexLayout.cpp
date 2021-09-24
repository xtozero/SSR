#include "stdafx.h"
#include "D3D11VetexLayout.h"

#include "D3D11Api.h"
#include "D3D11FlagConvertor.h"
#include "D3D11Shaders.h"

namespace
{
	void ConvertVertexLayoutToInputLayout( std::vector<std::string>& namePool, std::vector<D3D11_INPUT_ELEMENT_DESC>& descs, const VERTEX_LAYOUT_TRAIT* trait, uint32 size )
	{
		for ( uint32 i = 0; i < size; ++i )
		{
			namePool.emplace_back( trait[i].m_name );
			descs.emplace_back( );

			D3D11_INPUT_ELEMENT_DESC& desc = descs.back( );

			desc.SemanticName = namePool[i].c_str( );
			desc.SemanticIndex = trait[i].m_index;
			desc.Format = ConvertFormatToDxgiFormat( trait[i].m_format );
			desc.InputSlot = trait[i].m_slot;
			desc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			desc.InputSlotClass = trait[i].m_isInstanceData ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
			desc.InstanceDataStepRate = trait[i].m_instanceDataStep;
		}
	}
}

namespace aga
{
	ID3D11InputLayout* D3D11VertexLayout::Resource( )
	{
		return m_pInputLayout;
	}

	const ID3D11InputLayout* D3D11VertexLayout::Resource( ) const
	{
		return m_pInputLayout;
	}

	D3D11VertexLayout::D3D11VertexLayout( const D3D11VertexShader* vs, const VERTEX_LAYOUT_TRAIT* trait, uint32 size )
	{
		m_namePool.reserve( size );
		m_inputDesc.reserve( size );
		ConvertVertexLayoutToInputLayout( m_namePool, m_inputDesc, trait, size );

		[[maybe_unused]] bool result = SUCCEEDED( D3D11Device( ).CreateInputLayout( m_inputDesc.data( ), size, vs->ByteCode( ), vs->ByteCodeSize( ), &m_pInputLayout ) );
		assert( result );
	}

	void D3D11VertexLayout::FreeResource( )
	{
		if ( m_pInputLayout )
		{
			m_pInputLayout->Release( );
			m_pInputLayout = nullptr;
		}
	}
}
