#pragma once

#include "PipelineState.h"
#include "SizedTypes.h"

#include <d3d11.h>

namespace aga
{
	class D3D11BlendState : public BlendState
	{
	public:
		ID3D11BlendState* Resource( );
		const ID3D11BlendState* Resource( ) const;

		const float* GetBlendFactor( ) const;

		uint32 SamplerMask( ) const;

		void SetBlendFactor( const float( &blendFactor )[4] );

		void SetSampleMask( uint32 sampleMask );

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
		uint32 m_sampleMask = D3D11_DEFAULT_SAMPLE_MASK;
		D3D11_BLEND_DESC m_desc;
	};
}