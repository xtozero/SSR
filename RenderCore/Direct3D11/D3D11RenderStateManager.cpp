#include "stdafx.h"
#include "D3D11RenderStateManager.h"

#include <d3d11.h>

void CD3D11RenderStateManager::SetInputLayout( ID3D11InputLayout* pInputLayout )
{
	if ( m_pCurInputLayout != pInputLayout )
	{
		m_pCurInputLayout = pInputLayout;
		m_deviceContext.IASetInputLayout( pInputLayout );
	}
}

void CD3D11RenderStateManager::SetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY topology )
{
	if ( m_curTopology != topology )
	{
		m_curTopology = topology;
		m_deviceContext.IASetPrimitiveTopology( topology );
	}
}

void CD3D11RenderStateManager::SetVertexBuffer( UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppVertexBuffers, const UINT* pStrides, const UINT* pOffsets )
{
	m_deviceContext.IASetVertexBuffers( StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets );
}

void CD3D11RenderStateManager::SetIndexBuffer( ID3D11Buffer* pIndexBuffer, DXGI_FORMAT Format, UINT offset )
{
	m_deviceContext.IASetIndexBuffer( pIndexBuffer, Format, offset );
}

void CD3D11RenderStateManager::SetVertexShader( ID3D11VertexShader* pVertexSahder, ID3D11ClassInstance* const* ppClassInstance, UINT NumClassInstances )
{
	if ( m_pCurVS != pVertexSahder )
	{
		m_pCurVS = pVertexSahder;
		m_deviceContext.VSSetShader( pVertexSahder, ppClassInstance, NumClassInstances );
	}
}

void CD3D11RenderStateManager::SetPixelShader( ID3D11PixelShader* pPixelShader, ID3D11ClassInstance* const* ppClassInstance, UINT NumClassInstances )
{
	if ( m_pCurPS != pPixelShader )
	{
		m_pCurPS = pPixelShader;
		m_deviceContext.PSSetShader( pPixelShader, ppClassInstance, NumClassInstances );
	}
}

void CD3D11RenderStateManager::SetComputeShader( ID3D11ComputeShader* pComputeShader, ID3D11ClassInstance* const* ppClassInstance, UINT NumClassInstances )
{
	if ( m_pCurCS != pComputeShader )
	{
		m_pCurCS = pComputeShader;
		m_deviceContext.CSSetShader( pComputeShader, ppClassInstance, NumClassInstances );
	}
}

void CD3D11RenderStateManager::SetVsShaderResource( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews )
{
	m_deviceContext.VSSetShaderResources( StartSlot, NumViews, ppShaderResourceViews );
}

void CD3D11RenderStateManager::SetPsShaderResource( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView * const * ppShaderResourceViews )
{
	int diffCount = 0;
	for ( int i = 0, slot = StartSlot, end = StartSlot + NumViews; slot < end; ++i, ++slot )
	{
		diffCount += ( ppShaderResourceViews[i] != m_pCurPSSRV[slot] ) ? 1 : 0;
		m_pCurPSSRV[slot] = ppShaderResourceViews[i];
	}

	if ( diffCount > 0 )
	{
		m_deviceContext.PSSetShaderResources( StartSlot, NumViews, ppShaderResourceViews );
	}
}

void CD3D11RenderStateManager::SetCsShaderResource( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews )
{
	m_deviceContext.CSSetShaderResources( StartSlot, NumViews, ppShaderResourceViews );
}

void CD3D11RenderStateManager::SetCsUnorderedAccess( UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView* const* ppUnorderedAccessViews, const UINT* pUAVInitialCounts )
{
	m_deviceContext.CSSetUnorderedAccessViews( StartSlot, NumUAVs, ppUnorderedAccessViews, pUAVInitialCounts );
}

void CD3D11RenderStateManager::SetVsConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers )
{
	int diffCount = 0;
	for ( int i = 0, slot = StartSlot, end = StartSlot + NumBuffers; slot < end; ++i, ++slot )
	{
		diffCount += ( ppConstantBuffers[i] != m_pCurVsBuffer[slot] ) ? 1 : 0;
		m_pCurVsBuffer[slot] = ppConstantBuffers[i];
	}

	if ( diffCount > 0 )
	{
		m_deviceContext.VSSetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers );
	}
}

void CD3D11RenderStateManager::SetPsConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers )
{
	int diffCount = 0;
	for ( int i = 0, slot = StartSlot, end = StartSlot + NumBuffers; slot < end; ++i, ++slot )
	{
		diffCount += ( ppConstantBuffers[i] != m_pCurPsBuffer[slot] ) ? 1 : 0;
		m_pCurPsBuffer[slot] = ppConstantBuffers[i];
	}

	if ( diffCount > 0 )
	{
		m_deviceContext.PSSetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers );
	}
}

void CD3D11RenderStateManager::SetCsConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers )
{
	int diffCount = 0;
	for ( int i = 0, slot = StartSlot, end = StartSlot + NumBuffers; slot < end; ++i, ++slot )
	{
		diffCount += ( ppConstantBuffers[i] != m_pCurCsBuffer[slot] ) ? 1 : 0;
		m_pCurCsBuffer[slot] = ppConstantBuffers[i];
	}

	if ( diffCount > 0 )
	{
		m_deviceContext.CSSetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers );
	}
}

void CD3D11RenderStateManager::SetVsSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers )
{
	int diffCount = 0;
	for ( int i = 0, slot = StartSlot, end = StartSlot + NumSamplers; slot < end; ++i, ++slot )
	{
		diffCount += ( ppSamplers[i] != m_pCurVsSampler[slot] ) ? 1 : 0;
		m_pCurVsSampler[slot] = ppSamplers[i];
	}

	if ( diffCount > 0 )
	{
		m_deviceContext.VSSetSamplers( StartSlot, NumSamplers, ppSamplers );
	}
}

void CD3D11RenderStateManager::SetHsSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers )
{
	int diffCount = 0;
	for ( int i = 0, slot = StartSlot, end = StartSlot + NumSamplers; slot < end; ++i, ++slot )
	{
		diffCount += ( ppSamplers[i] != m_pCurHsSampler[slot] ) ? 1 : 0;
		m_pCurHsSampler[slot] = ppSamplers[i];
	}

	if ( diffCount > 0 )
	{
		m_deviceContext.HSSetSamplers( StartSlot, NumSamplers, ppSamplers );
	}
}

void CD3D11RenderStateManager::SetDsSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers )
{
	int diffCount = 0;
	for ( int i = 0, slot = StartSlot, end = StartSlot + NumSamplers; slot < end; ++i, ++slot )
	{
		diffCount += ( ppSamplers[i] != m_pCurDsSampler[slot] ) ? 1 : 0;
		m_pCurDsSampler[slot] = ppSamplers[i];
	}

	if ( diffCount > 0 )
	{
		m_deviceContext.DSSetSamplers( StartSlot, NumSamplers, ppSamplers );
	}
}

void CD3D11RenderStateManager::SetGsSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers )
{
	int diffCount = 0;
	for ( int i = 0, slot = StartSlot, end = StartSlot + NumSamplers; slot < end; ++i, ++slot )
	{
		diffCount += ( ppSamplers[i] != m_pCurGsSampler[slot] ) ? 1 : 0;
		m_pCurGsSampler[slot] = ppSamplers[i];
	}

	if ( diffCount > 0 )
	{
		m_deviceContext.GSSetSamplers( StartSlot, NumSamplers, ppSamplers );
	}
}

void CD3D11RenderStateManager::SetPsSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers )
{
	int diffCount = 0;
	for ( int i = 0, slot = StartSlot, end = StartSlot + NumSamplers; slot < end; ++i, ++slot )
	{
		diffCount += ( ppSamplers[i] != m_pCurPsSampler[slot] ) ? 1 : 0;
		m_pCurPsSampler[slot] = ppSamplers[i];
	}

	if ( diffCount > 0 )
	{
		m_deviceContext.PSSetSamplers( StartSlot, NumSamplers, ppSamplers );
	}
}

void CD3D11RenderStateManager::SetCsSamplers( UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers )
{
	int diffCount = 0;
	for ( int i = 0, slot = StartSlot, end = StartSlot + NumSamplers; slot < end; ++i, ++slot )
	{
		diffCount += ( ppSamplers[i] != m_pCurCsSampler[slot] ) ? 1 : 0;
		m_pCurCsSampler[slot] = ppSamplers[i];
	}

	if ( diffCount > 0 )
	{
		m_deviceContext.CSSetSamplers( StartSlot, NumSamplers, ppSamplers );
	}
}

void CD3D11RenderStateManager::SetRasterizerState( ID3D11RasterizerState * pRasterizerState )
{
	if ( m_pCurRasterizerState != pRasterizerState )
	{
		m_pCurRasterizerState = pRasterizerState;
		m_deviceContext.RSSetState( pRasterizerState );
	}
}

void CD3D11RenderStateManager::SetBlendState( ID3D11BlendState* pBlendState, const float BlendFactor[4], UINT SampleMask )
{
	if ( m_pCurBlendState != pBlendState )
	{
		m_pCurBlendState = pBlendState;
		m_deviceContext.OMSetBlendState( pBlendState, BlendFactor, SampleMask );
	}
}

void CD3D11RenderStateManager::SetDepthStencilState( ID3D11DepthStencilState* pDepthStencilState, UINT StencilRef )
{
	if ( m_pCurDepthStencilState != pDepthStencilState || m_curStencilRef != StencilRef )
	{
		m_pCurDepthStencilState = pDepthStencilState;
		m_curStencilRef = StencilRef;
		m_deviceContext.OMSetDepthStencilState( pDepthStencilState, StencilRef );
	}
}

HRESULT CD3D11RenderStateManager::Map( ID3D11Resource* pResource, UINT Subresource, D3D11_MAP MapType, UINT MapFlags, D3D11_MAPPED_SUBRESOURCE* pMappedResource )
{
	return m_deviceContext.Map( pResource, Subresource, MapType, MapFlags, pMappedResource );
}

void CD3D11RenderStateManager::Unmap( ID3D11Resource* pResource, UINT Subresource )
{
	m_deviceContext.Unmap( pResource, Subresource );
}

void CD3D11RenderStateManager::Dispatch( UINT x, UINT y, UINT z )
{
	m_deviceContext.Dispatch( x, y, z );
}
