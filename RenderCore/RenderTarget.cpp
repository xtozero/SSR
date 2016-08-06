#include "stdafx.h"
#include "RenderTarget.h"

#include <d3d11.h>

bool CRenderTarget::CreateRenderTarget( ID3D11Device* pDevice, ID3D11Resource* pResource, const D3D11_RENDER_TARGET_VIEW_DESC* rtvDesc )
{
	if ( pDevice )
	{
		if ( SUCCEEDED( pDevice->CreateRenderTargetView( pResource, rtvDesc, &m_pRenderTargetView ) ) )
		{
			return true;
		}
	}

	return false;
}

ID3D11RenderTargetView* CRenderTarget::Get( ) const
{
	return m_pRenderTargetView.Get( );
}

void CRenderTarget::SetRenderTargetView( Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& renderTargetView )
{
	m_pRenderTargetView = renderTargetView;
}

void CRenderTarget::Clear( ID3D11DeviceContext* pDeviceContext, const float (&clearColor)[4] )
{
	if ( pDeviceContext && m_pRenderTargetView )
	{
		pDeviceContext->ClearRenderTargetView( m_pRenderTargetView.Get( ), clearColor );
	}
}

CRenderTarget::CRenderTarget( ) : m_pRenderTargetView( nullptr )
{

}