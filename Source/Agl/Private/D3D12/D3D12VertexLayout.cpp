#include "D3D12VertexLayout.h"

#include "D3D12FlagConvertor.h"

using ::agl::ConvertFormatToDxgiFormat;
using ::agl::VertexLayoutData;

namespace
{
	void ConvertVertexLayoutToInputLayout( std::vector<D3D12_INPUT_ELEMENT_DESC>& descs, const VertexLayoutData* layoutData, uint32 size )
	{
		for ( uint32 i = 0; i < size; ++i )
		{
			descs.emplace_back();

			D3D12_INPUT_ELEMENT_DESC& desc = descs.back();

			desc.SemanticName = layoutData[i].m_name.Str().data();
			desc.SemanticIndex = layoutData[i].m_index;
			desc.Format = ConvertFormatToDxgiFormat( layoutData[i].m_format );
			desc.InputSlot = layoutData[i].m_slot;
			desc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
			desc.InputSlotClass = layoutData[i].m_isInstanceData ? D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			desc.InstanceDataStepRate = layoutData[i].m_instanceDataStep;
		}
	}
}

namespace agl
{
	const std::vector<D3D12_INPUT_ELEMENT_DESC>& D3D12VertexLayout::GetD3DDescs() const
	{
		return m_inputDesc;
	}

	D3D12VertexLayout::D3D12VertexLayout( const VertexLayoutData* layoutData, uint32 size )
	{
		m_inputDesc.reserve( size );
		ConvertVertexLayoutToInputLayout( m_inputDesc, layoutData, size );
	}
}
