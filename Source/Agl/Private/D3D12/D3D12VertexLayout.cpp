#include "D3D12VertexLayout.h"

#include "D3D12FlagConvertor.h"

using ::agl::ConvertFormatToDxgiFormat;
using ::agl::VERTEX_LAYOUT_TRAIT;

namespace
{
	void ConvertVertexLayoutToInputLayout( std::vector<D3D12_INPUT_ELEMENT_DESC>& descs, const VERTEX_LAYOUT_TRAIT* trait, uint32 size )
	{
		for ( uint32 i = 0; i < size; ++i )
		{
			descs.emplace_back();

			D3D12_INPUT_ELEMENT_DESC& desc = descs.back();

			desc.SemanticName = trait[i].m_name.Str().data();
			desc.SemanticIndex = trait[i].m_index;
			desc.Format = ConvertFormatToDxgiFormat( trait[i].m_format );
			desc.InputSlot = trait[i].m_slot;
			desc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
			desc.InputSlotClass = trait[i].m_isInstanceData ? D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			desc.InstanceDataStepRate = trait[i].m_instanceDataStep;
		}
	}
}

namespace agl
{
	const std::vector<D3D12_INPUT_ELEMENT_DESC>& D3D12VertexLayout::GetDesc() const
	{
		return m_inputDesc;
	}

	D3D12VertexLayout::D3D12VertexLayout( const VERTEX_LAYOUT_TRAIT* trait, uint32 size )
	{
		m_inputDesc.reserve( size );
		ConvertVertexLayoutToInputLayout( m_inputDesc, trait, size );
	}
}
