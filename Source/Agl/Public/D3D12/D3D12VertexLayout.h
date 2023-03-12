#pragma once

#include "PipelineState.h"

#include <d3d12.h>
#include <vector>

namespace agl
{
	class D3D12VertexLayout : public VertexLayout
	{
	public:
		const std::vector<D3D12_INPUT_ELEMENT_DESC>& GetDesc() const;

		D3D12VertexLayout( const VertexLayoutTrait* trait, uint32 size );
		D3D12VertexLayout( const D3D12VertexLayout& ) = delete;
		D3D12VertexLayout( D3D12VertexLayout&& ) = delete;
		D3D12VertexLayout& operator=( const D3D12VertexLayout& ) = delete;
		D3D12VertexLayout& operator=( D3D12VertexLayout&& ) = delete;

	private:
		virtual void InitResource() override {}
		virtual void FreeResource() override {}

		std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputDesc;
	};
}
