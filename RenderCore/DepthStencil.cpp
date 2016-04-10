#include "stdafx.h"
#include "DepthStencil.h"
#include "ITexture.h"

#include <d3d11.h>

bool CDepthStencil::CreateDepthStencil( ID3D11Device* pDevice, std::shared_ptr<ITexture>& pTexture, const D3D11_DEPTH_STENCIL_VIEW_DESC* dsvDesc )
{
	if ( pDevice && pTexture )
	{
		if ( SUCCEEDED( pDevice->CreateDepthStencilView( pTexture->Get(), dsvDesc, &m_pDepthStencilVeiw ) ) )
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

CDepthStencil::CDepthStencil( ) : m_pDepthStencilVeiw( nullptr )
{

}