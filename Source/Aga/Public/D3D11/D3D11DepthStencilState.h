#pragma once

#include "PipelineState.h"

#include <d3d11.h>

namespace aga
{
	class D3D11DepthStencilState : public DepthStencilState
	{
	public:
		ID3D11DepthStencilState* Resource( );
		const ID3D11DepthStencilState* Resource( ) const;

		unsigned int GetStencilRef( ) const;
		void SetStencilRef( unsigned int stencilRef );

		D3D11DepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait );
		D3D11DepthStencilState( const D3D11DepthStencilState& ) = delete;
		D3D11DepthStencilState( D3D11DepthStencilState&& ) = default;
		D3D11DepthStencilState& operator=( const D3D11DepthStencilState& ) = delete;
		D3D11DepthStencilState& operator=( D3D11DepthStencilState&& ) = default;
		~D3D11DepthStencilState( ) = default;

	private:
		virtual void InitResource( ) override;
		virtual void FreeResource( ) override;

		unsigned int m_stencilRef = 0;
		D3D11_DEPTH_STENCIL_DESC m_desc;
		ID3D11DepthStencilState* m_depthStencilState = nullptr;
	};
}
