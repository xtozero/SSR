#pragma once

#include "PipelineState.h"

#include <d3d11.h>

namespace aga
{
	class D3D11BlendState : public BlendState
	{
	public:
		const float* GetBlendFactor( ) const { return m_blendFactor; }

		unsigned int GetSamplerMask( ) const { return m_sampleMask; }

		void SetBlendFactor( const float (&blendFactor)[4] ) noexcept
		{
			constexpr int size = std::extent_v<decltype( blendFactor )>;
			for ( int i = 0; i < size; ++i )
			{
				m_blendFactor[i] = blendFactor[i];
			}
		}

		void SetSampleMask( unsigned int sampleMask ) noexcept { m_sampleMask = sampleMask; }

		D3D11BlendState( const BLEND_STATE_TRAIT& trait );
		D3D11BlendState( const D3D11BlendState& ) = delete;
		D3D11BlendState( D3D11BlendState&& ) = default;
		D3D11BlendState& operator=( const D3D11BlendState& ) = delete;
		D3D11BlendState& operator=( D3D11BlendState&& ) = default;
		~D3D11BlendState( ) = default;

	private:
		virtual void InitResource( ) override;
		virtual void FreeResource( ) override;

		ID3D11BlendState* m_blendState = nullptr;

		float m_blendFactor[4] = {};
		unsigned int m_sampleMask = D3D11_DEFAULT_SAMPLE_MASK;
		D3D11_BLEND_DESC m_desc;
	};
}