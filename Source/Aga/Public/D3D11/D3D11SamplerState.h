#pragma once

#include "D3D11ResourceInterface.h"


class CD3D11SamplerState : public DeviceDependantBase<ID3D11SamplerState>
{
public:
	virtual void InitResource( ) override;

	CD3D11SamplerState( const SAMPLER_STATE_TRAIT& trait );
	CD3D11SamplerState( const CD3D11SamplerState& ) = default;
	CD3D11SamplerState( CD3D11SamplerState&& ) = default;
	CD3D11SamplerState& operator=( const CD3D11SamplerState& ) = default;
	CD3D11SamplerState& operator=( CD3D11SamplerState&& ) = default;
	~CD3D11SamplerState( ) = default;

private:
	D3D11_SAMPLER_DESC m_desc;
};