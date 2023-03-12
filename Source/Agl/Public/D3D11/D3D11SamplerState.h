#pragma once

#include "PipelineState.h"

#include <d3d11.h>

namespace agl
{
	class D3D11SamplerState : public SamplerState
	{
	public:
		ID3D11SamplerState* Resource();
		const ID3D11SamplerState* Resource() const;

		D3D11SamplerState( const SamplerStateTrait& trait );
		D3D11SamplerState( const D3D11SamplerState& ) = default;
		D3D11SamplerState( D3D11SamplerState&& ) = default;
		D3D11SamplerState& operator=( const D3D11SamplerState& ) = default;
		D3D11SamplerState& operator=( D3D11SamplerState&& ) = default;
		virtual ~D3D11SamplerState() override = default;

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		ID3D11SamplerState* m_samplerState = nullptr;

		D3D11_SAMPLER_DESC m_desc;
	};
}
