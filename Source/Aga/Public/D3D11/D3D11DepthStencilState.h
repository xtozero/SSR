#pragma once

#include "D3D11ResourceInterface.h"

class CD3D11DepthStencilState : public DeviceDependantBase<ID3D11DepthStencilState>
{
public:
	virtual void InitResource( ) override;

	unsigned int GetStencilRef( ) const { return m_stencilRef; }
	void SetStencilRef( unsigned int stencilRef ) { m_stencilRef = stencilRef; }

	CD3D11DepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait );
	CD3D11DepthStencilState( const CD3D11DepthStencilState& ) = delete;
	CD3D11DepthStencilState( CD3D11DepthStencilState&& ) = default;
	CD3D11DepthStencilState& operator=( const CD3D11DepthStencilState& ) = delete;
	CD3D11DepthStencilState& operator=( CD3D11DepthStencilState&& ) = default;
	~CD3D11DepthStencilState( ) = default;

private:
	unsigned int m_stencilRef = 0;
	D3D11_DEPTH_STENCIL_DESC m_desc;
};