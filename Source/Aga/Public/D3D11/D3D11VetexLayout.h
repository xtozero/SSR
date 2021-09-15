#pragma once

#include "PipelineState.h"
#include "SizedTypes.h"

#include <d3d11.h>
#include <vector>

namespace aga
{
	class D3D11VertexShader;

	class D3D11VertexLayout : public VertexLayout
	{
	public:
		ID3D11InputLayout* Resource( );
		const ID3D11InputLayout* Resource( ) const;

		D3D11VertexLayout( const D3D11VertexShader* vs, const VERTEX_LAYOUT_TRAIT* trait, uint32 size );
		D3D11VertexLayout( const D3D11VertexLayout& ) = delete;
		D3D11VertexLayout( D3D11VertexLayout&& ) = delete;
		D3D11VertexLayout& operator=( const D3D11VertexLayout& ) = delete;
		D3D11VertexLayout& operator=( D3D11VertexLayout&& ) = delete;

	private:
		virtual void InitResource( ) override { }
		virtual void FreeResource( ) override;
		
		std::vector<std::string> m_namePool;
		std::vector<D3D11_INPUT_ELEMENT_DESC> m_inputDesc;
		ID3D11InputLayout* m_pInputLayout = nullptr;
	};
}
