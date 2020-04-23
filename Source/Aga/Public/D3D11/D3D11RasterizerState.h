#pragma once

#include "D3D11ResourceInterface.h"

class CD3D11RasterizerState : public DeviceDependantBase<ID3D11RasterizerState>
{
public:
	virtual void InitResource( ) override;

	CD3D11RasterizerState( const RASTERIZER_STATE_TRAIT& trait );
	CD3D11RasterizerState( const CD3D11RasterizerState& ) = delete;
	CD3D11RasterizerState( CD3D11RasterizerState&& ) = default;
	CD3D11RasterizerState& operator=( const CD3D11RasterizerState& ) = delete;
	CD3D11RasterizerState& operator=( CD3D11RasterizerState&& ) = default;
	~CD3D11RasterizerState( ) = default;

private:
	D3D11_RASTERIZER_DESC m_desc;
};