#include "stdafx.h"
#include "D3D11/D3D11RenderStateManager.h"

#include <d3d11.h>

void CD3D11RenderStateManager::SetInputLayout( ID3D11InputLayout* pInputLayout )
{
	if ( m_pCurInputLayout != pInputLayout )
	{
		m_pCurInputLayout = pInputLayout;
		m_pDeviceContext->IASetInputLayout( pInputLayout );
	}
}

void CD3D11RenderStateManager::SetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY topology )
{
	if ( m_curTopology != topology )
	{
		m_curTopology = topology;
		m_pDeviceContext->IASetPrimitiveTopology( topology );
	}
}

void CD3D11RenderStateManager::SetVertexBuffer( UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppVertexBuffers, const UINT* pStrides, const UINT* pOffsets )
{
	m_pDeviceContext->IASetVertexBuffers( StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets );
}

void CD3D11RenderStateManager::SetIndexBuffer( ID3D11Buffer* pIndexBuffer, DXGI_FORMAT Format, UINT offset )
{
	m_pDeviceContext->IASetIndexBuffer( pIndexBuffer, Format, offset );
}

void CD3D11RenderStateManager::SetVertexShader( ID3D11VertexShader* pVertexSahder, ID3D11ClassInstance* const* ppClassInstance, UINT NumClassInstances )
{
	if ( m_pCurVS != pVertexSahder )
	{
		m_pCurVS = pVertexSahder;
		m_pDeviceContext->VSSetShader( pVertexSahder, ppClassInstance, NumClassInstances );
	}
}

void CD3D11RenderStateManager::SetGeometryShader( ID3D11GeometryShader * pGeometryShader, ID3D11ClassInstance * const * ppClassInstance, UINT NumClassInstances )
{
	if ( m_pCurGS != pGeometryShader )
	{
		m_pCurGS = pGeometryShader;
		m_pDeviceContext->GSSetShader( pGeometryShader, ppClassInstance, NumClassInstances );
	}
}

void CD3D11RenderStateManager::SetPixelShader( ID3D11PixelShader* pPixelShader, ID3D11ClassInstance* const* ppClassInstance, UINT NumClassInstances )
{
	if ( m_pCurPS != pPixelShader )
	{
		m_pCurPS = pPixelShader;
		m_pDeviceContext->PSSetShader( pPixelShader, ppClassInstance, NumClassInstances );
	}
}

void CD3D11RenderStateManager::SetComputeShader( ID3D11ComputeShader* pComputeShader, ID3D11ClassInstance* const* ppClassInstance, UINT NumClassInstances )
{
	if ( m_pCurCS != pComputeShader )
	{
		m_pCurCS = pComputeShader;
		m_pDeviceContext->CSSetShader( pComputeShader, ppClassInstance, NumClassInstances );
	}
}

void CD3D11RenderStateManager::SetVsShaderResource( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews )
{
	int diffCount = 0;
	for ( int i = 0, slot = StartSlot, end = StartSlot + NumViews; slot < end; ++i, ++slot )
	{
		diffCount += ( ppShaderResourceViews[i] != m_pCurVsSRV[slot] ) ? 1 : 0;
		m_pCurVsSRV[slot] = ppShaderResourceViews[i];
	}

	if ( diffCount > 0 )
	{
		m_pDeviceContext->VSSetShaderResources( StartSlot, NumViews, ppShaderResourceViews );
	}
}

void CD3D11RenderStateManager::SetPsShaderResource( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView * const * ppShaderResourceViews )
{
	int diffCount = 0;
	for ( int i = 0, slot = StartSlot, end = StartSlot + NumViews; slot < end; ++i, ++slot )
	{
		diffCount += ( ppShaderResourceViews[i] != m_pCurPsSRV[slot] ) ? 1 : 0;
		m_pCurPsSRV[slot] = ppShaderResourceViews[i];
	}

	if ( diffCount > 0 )
	{
		m_pDeviceContext->PSSetShaderResources( StartSlot, NumViews, ppShaderResourceViews );
	}
}

void CD3D11RenderStateManager::SetCsShaderResource( UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView* const* ppShaderResourceViews )
{
	int diffCount = 0;
	for ( int i = 0, slot = StartSlot, end = StartSlot + NumViews; slot < end; ++i, ++slot )
	{
		diffCount += ( ppShaderResourceViews[i] != m_pCurCsSRV[slot] ) ? 1 : 0;
		m_pCurCsSRV[slot] = ppShaderResourceViews[i];
	}

	if ( diffCount > 0 )
	{
		m_pDeviceContext->CSSetShaderResources( StartSlot, NumViews, ppShaderResourceViews );
	}
}

void CD3D11RenderStateManager::SetCsUnorderedAccess( UINT StartSlot, UINT NumUAVs, ID3D11UnorderedAccessView* const* ppUnorderedAccessViews, const UINT* pUAVInitialCounts )
{
	m_pDeviceContext->CSSetUnorderedAccessViews( StartSlot, NumUAVs, ppUnorderedAccessViews, pUAVInitialCounts );
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
		m_pDeviceContext->VSSetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers );
	}
}

void CD3D11RenderStateManager::SetGsConstantBuffers( UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers )
{
	int diffCount = 0;
	for ( int i = 0, slot = StartSlot, end = StartSlot + NumBuffers; slot < end; ++i, ++slot )
	{
		diffCount += ( ppConstantBuffers[i] != m_pCurGsBuffer[slot] ) ? 1 : 0;
		m_pCurGsBuffer[slot] = ppConstantBuffers[i];
	}

	if ( diffCount > 0 )
	{
		m_pDeviceContext->GSSetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers );
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
		m_pDeviceContext->PSSetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers );
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
		m_pDeviceContext->CSSetConstantBuffers( StartSlot, NumBuffers, ppConstantBuffers );
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
		m_pDeviceContext->VSSetSamplers( StartSlot, NumSamplers, ppSamplers );
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
		m_pDeviceContext->HSSetSamplers( StartSlot, NumSamplers, ppSamplers );
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
		m_pDeviceContext->DSSetSamplers( StartSlot, NumSamplers, ppSamplers );
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
		m_pDeviceContext->GSSetSamplers( StartSlot, NumSamplers, ppSamplers );
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
		m_pDeviceContext->PSSetSamplers( StartSlot, NumSamplers, ppSamplers );
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
		m_pDeviceContext->CSSetSamplers( StartSlot, NumSamplers, ppSamplers );
	}
}

void CD3D11RenderStateManager::SetRasterizerState( ID3D11RasterizerState * pRasterizerState )
{
	if ( m_pCurRasterizerState != pRasterizerState )
	{
		m_pCurRasterizerState = pRasterizerState;
		m_pDeviceContext->RSSetState( pRasterizerState );
	}
}

void CD3D11RenderStateManager::SetBlendState( ID3D11BlendState* pBlendState, const float BlendFactor[4], UINT SampleMask )
{
	if ( m_pCurBlendState != pBlendState )
	{
		m_pCurBlendState = pBlendState;
		m_pDeviceContext->OMSetBlendState( pBlendState, BlendFactor, SampleMask );
	}
}

void CD3D11RenderStateManager::SetDepthStencilState( ID3D11DepthStencilState* pDepthStencilState, UINT StencilRef )
{
	if ( m_pCurDepthStencilState != pDepthStencilState || m_curStencilRef != StencilRef )
	{
		m_pCurDepthStencilState = pDepthStencilState;
		m_curStencilRef = StencilRef;
		m_pDeviceContext->OMSetDepthStencilState( pDepthStencilState, StencilRef );
	}
}

void CD3D11RenderStateManager::SetRenderTargets( UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews, ID3D11DepthStencilView* pDepthStencilView )
{
	m_pDeviceContext->OMSetRenderTargets( NumViews, ppRenderTargetViews, pDepthStencilView );
}
