#pragma once

#include "D3D11ResourceInterface.h"

class CD3D11BlendState : public DeviceDependantBase<ID3D11BlendState>
{
public:
	virtual void InitResource( ) override;

	const float* GetBlendFactor( ) const { return m_blendFactor.data( ); }

	unsigned int GetSamplerMask( ) const { return m_sampleMask; }

	void SetBlendFactor( const std::array<float, 4>& blendFactor ) noexcept
	{
		m_blendFactor = blendFactor;
	}

	void SetSampleMask( unsigned int sampleMask ) noexcept { m_sampleMask = sampleMask; }

	CD3D11BlendState( const BLEND_STATE_TRAIT& trait );
	CD3D11BlendState( const CD3D11BlendState& ) = delete;
	CD3D11BlendState( CD3D11BlendState&& ) = default;
	CD3D11BlendState& operator=( const CD3D11BlendState& ) = delete;
	CD3D11BlendState& operator=( CD3D11BlendState&& ) = default;
	~CD3D11BlendState( ) = default;

private:
	std::array<float, 4> m_blendFactor = { 0, };
	unsigned int m_sampleMask = D3D11_DEFAULT_SAMPLE_MASK;
	D3D11_BLEND_DESC m_desc;
};