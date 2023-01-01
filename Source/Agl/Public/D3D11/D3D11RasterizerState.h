#pragma once

#include "PipelineState.h"

#include <d3d11.h>

namespace agl
{
	class D3D11RasterizerState : public RasterizerState
	{
	public:
		ID3D11RasterizerState* Resource();
		const ID3D11RasterizerState* Resource() const;

		D3D11RasterizerState( const RASTERIZER_STATE_TRAIT& trait );
		D3D11RasterizerState( const D3D11RasterizerState& ) = delete;
		D3D11RasterizerState( D3D11RasterizerState&& ) = default;
		D3D11RasterizerState& operator=( const D3D11RasterizerState& ) = delete;
		D3D11RasterizerState& operator=( D3D11RasterizerState&& ) = default;
		virtual ~D3D11RasterizerState() override = default;

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		D3D11_RASTERIZER_DESC m_desc;
		ID3D11RasterizerState* m_rasterizerState = nullptr;
	};
}
