#include "stdafx.h"
#include "DepthStencil.h"

#include <d3d11.h>

bool CDepthStencil::CreateDepthStencil( ID3D11Device* pDevice, ID3D11Resource* pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC* dsvDesc )
{
	if ( pDevice && pResource )
	{
		if ( SUCCEEDED( pDevice->CreateDepthStencilView( pResource, dsvDesc, &m_pDepthStencilVeiw ) ) )
		{
			return true;
		}
	}

	return false;
}

ID3D11DepthStencilView* CDepthStencil::Get( ) const
{
	return m_pDepthStencilVeiw.Get( );
}

void CDepthStencil::SetRenderTargetView( Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& depthStencilView )
{
	m_pDepthStencilVeiw = depthStencilView;
}

void CDepthStencil::Clear( ID3D11DeviceContext* pDeviceContext, unsigned int clearFlag, float depth, unsigned char stencil )
{
	if ( pDeviceContext && m_pDepthStencilVeiw )
	{
		pDeviceContext->ClearDepthStencilView( m_pDepthStencilVeiw.Get( ), clearFlag, depth, stencil );
	}
}