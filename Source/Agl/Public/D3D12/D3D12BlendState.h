#pragma once

#include "PipelineState.h"
#include "SizedTypes.h"

#include <d3d12.h>

namespace agl
{
	class D3D12BlendState final : public BlendState
	{
	public:
		const D3D12_BLEND_DESC& GetDesc() const
		{
			return m_desc;
		}

		const float* GetBlendFactor() const;

		uint32 SamplerMask() const;

		void SetBlendFactor( const float( &blendFactor )[4] );

		void SetSampleMask( uint32 sampleMask );

		D3D12BlendState( const BlendStateTrait& trait );
		D3D12BlendState( const D3D12BlendState& ) = default;
		D3D12BlendState( D3D12BlendState&& ) = default;
		D3D12BlendState& operator=( const D3D12BlendState& ) = default;
		D3D12BlendState& operator=( D3D12BlendState&& ) = default;
		virtual ~D3D12BlendState() override = default;

	private:
		virtual void InitResource() override;
		virtual void FreeResource() override;

		float m_blendFactor[4] = {};
		uint32 m_sampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		D3D12_BLEND_DESC m_desc;
	};
}
