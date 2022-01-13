#include "stdafx.h"
#include "D3D11CommandList.h"

#include "D3D11Api.h"
#include "D3D11BaseTexture.h"
#include "D3D11Buffer.h"
#include "D3D11PipelineState.h"
#include "D3D11ResourceViews.h"
#include "D3D11SamplerState.h"

#include "ShaderBindings.h"

#include <cassert>
#include <d3d11.h>

namespace aga
{
	void D3D11ImmediateCommandList::BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets )
	{
		m_stateCache.BindVertexBuffer( D3D11Context(), vertexBuffers, startSlot, numBuffers, pOffsets );
	}

	void D3D11ImmediateCommandList::BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset )
	{
		m_stateCache.BindIndexBuffer( D3D11Context(), indexBuffer, indexOffset );
	}

	void D3D11ImmediateCommandList::BindPipelineState( PipelineState* pipelineState )
	{
		m_stateCache.BindPipelineState( D3D11Context(), pipelineState );
	}

	void D3D11ImmediateCommandList::BindShader( VertexShader* vs )
	{
		m_stateCache.BindShader( D3D11Context(), vs );
	}

	void D3D11ImmediateCommandList::BindShader( GeometryShader* gs )
	{
		m_stateCache.BindShader( D3D11Context(), gs );
	}

	void D3D11ImmediateCommandList::BindShader( PixelShader* ps )
	{
		m_stateCache.BindShader( D3D11Context(), ps );
	}

	void D3D11ImmediateCommandList::BindShader( ComputeShader* cs )
	{
		m_stateCache.BindShader( D3D11Context(), cs );
	}

	void D3D11ImmediateCommandList::BindShaderResources( const ShaderBindings& shaderBindings )
	{
		m_stateCache.BindShaderResources( D3D11Context(), shaderBindings );
	}

	void D3D11ImmediateCommandList::BindConstantBuffer( SHADER_TYPE shader, uint32 slot, Buffer* buffer )
	{
		m_stateCache.BindConstantBuffer( D3D11Context(), shader, slot, buffer );
	}

	void D3D11ImmediateCommandList::BindSRV( SHADER_TYPE shader, uint32 slot, ShaderResourceView* srv )
	{
		m_stateCache.BindSRV( D3D11Context(), shader, slot, srv );
	}

	void D3D11ImmediateCommandList::BindUAV( SHADER_TYPE shader, uint32 slot, UnorderedAccessView* uav )
	{
		m_stateCache.BindUAV( D3D11Context(), shader, slot, uav );
	}

	void D3D11ImmediateCommandList::BindSampler( SHADER_TYPE shader, uint32 slot, SamplerState* sampler )
	{
		m_stateCache.BindSampler( D3D11Context(), shader, slot, sampler );
	}

	void D3D11ImmediateCommandList::DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation )
	{
		D3D11Context().DrawInstanced( vertexCount, numInstance, baseVertexLocation, 0 );
	}

	void D3D11ImmediateCommandList::DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
		D3D11Context().DrawIndexedInstanced( indexCount, numInstance, startIndexLocation, baseVertexLocation, 0 );
	}

	void D3D11ImmediateCommandList::Dispatch( uint32 x, uint32 y, uint32 z )
	{
		D3D11Context().Dispatch( x, y, z );
	}

	void D3D11ImmediateCommandList::SetViewports( uint32 count, const CubeArea<float>* areas )
	{
		m_stateCache.SetViewports( D3D11Context(), count, areas );
	}

	void D3D11ImmediateCommandList::SetScissorRects( uint32 count, const RectangleArea<int32>* areas )
	{
		m_stateCache.SetScissorRects( D3D11Context(), count, areas );
	}

	void D3D11ImmediateCommandList::BindRenderTargets( aga::Texture** pRenderTargets, uint32 renderTargetCount, aga::Texture* depthStencil )
	{
		m_stateCache.BindRenderTargets( D3D11Context(), pRenderTargets, renderTargetCount, depthStencil );
	}

	void D3D11ImmediateCommandList::ClearRenderTarget( Texture* renderTarget, const float( &clearColor )[4] )
	{
		ID3D11RenderTargetView* rtvs = nullptr;

		if ( auto rtTex = static_cast<D3D11BaseTexture*>( renderTarget ) )
		{
			if ( auto d3d11RTV = static_cast<D3D11RenderTargetView*>( rtTex->RTV() ) )
			{
				rtvs = d3d11RTV->Resource();
			}
		}

		if ( rtvs == nullptr )
		{
			return;
		}

		D3D11Context().ClearRenderTargetView( rtvs, clearColor );
	}

	void D3D11ImmediateCommandList::ClearDepthStencil( Texture* depthStencil, float depthColor, UINT8 stencilColor )
	{
		ID3D11DepthStencilView* dsv = nullptr;

		if ( auto dsTex = static_cast<D3D11BaseTexture*>( depthStencil ) )
		{
			if ( auto d3d11DSV = static_cast<D3D11DepthStencilView*>( dsTex->DSV() ) )
			{
				dsv = d3d11DSV->Resource();
			}
		}

		if ( dsv == nullptr )
		{
			return;
		}

		D3D11Context().ClearDepthStencilView( dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depthColor, stencilColor );
	}

	void D3D11ImmediateCommandList::Execute( IDeferredCommandList& commandList )
	{
		D3D11DeferredCommandList& d3d11DeferredCommandList = static_cast<D3D11DeferredCommandList&>( commandList );
		d3d11DeferredCommandList.RequestExecute();
	}

	void D3D11DeferredCommandList::BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets )
	{
		m_stateCache.BindVertexBuffer( *m_pContext, vertexBuffers, startSlot, numBuffers, pOffsets );
	}

	void D3D11DeferredCommandList::BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset )
	{
		m_stateCache.BindIndexBuffer( *m_pContext, indexBuffer, indexOffset );
	}

	void D3D11DeferredCommandList::BindPipelineState( PipelineState* pipelineState )
	{
		m_stateCache.BindPipelineState( *m_pContext, pipelineState );
	}

	void D3D11DeferredCommandList::BindShader( VertexShader* vs )
	{
		m_stateCache.BindShader( *m_pContext, vs );
	}

	void D3D11DeferredCommandList::BindShader( GeometryShader* gs )
	{
		m_stateCache.BindShader( *m_pContext, gs );
	}

	void D3D11DeferredCommandList::BindShader( PixelShader* ps )
	{
		m_stateCache.BindShader( *m_pContext, ps );
	}

	void D3D11DeferredCommandList::BindShader( ComputeShader* cs )
	{
		m_stateCache.BindShader( *m_pContext, cs );
	}

	void D3D11DeferredCommandList::BindShaderResources( const ShaderBindings& shaderBindings )
	{
		m_stateCache.BindShaderResources( *m_pContext, shaderBindings );
	}

	void D3D11DeferredCommandList::BindConstantBuffer( SHADER_TYPE shader, uint32 slot, Buffer* buffer )
	{
		m_stateCache.BindConstantBuffer( *m_pContext, shader, slot, buffer );
	}

	void D3D11DeferredCommandList::BindSRV( SHADER_TYPE shader, uint32 slot, ShaderResourceView* srv )
	{
		m_stateCache.BindSRV( *m_pContext, shader, slot, srv );
	}

	void D3D11DeferredCommandList::BindUAV( SHADER_TYPE shader, uint32 slot, UnorderedAccessView* uav )
	{
		m_stateCache.BindUAV( *m_pContext, shader, slot, uav );
	}

	void D3D11DeferredCommandList::BindSampler( SHADER_TYPE shader, uint32 slot, SamplerState* sampler )
	{
		m_stateCache.BindSampler( *m_pContext, shader, slot, sampler );
	}

	void D3D11DeferredCommandList::DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation )
	{
		m_pContext->DrawInstanced( vertexCount, numInstance, baseVertexLocation, 0 );
	}

	void D3D11DeferredCommandList::DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
		m_pContext->DrawIndexedInstanced( indexCount, numInstance, startIndexLocation, baseVertexLocation, 0 );
	}

	void D3D11DeferredCommandList::Dispatch( uint32 x, uint32 y, uint32 z )
	{
		m_pContext->Dispatch( x, y, z );
	}

	void D3D11DeferredCommandList::SetViewports( uint32 count, const CubeArea<float>* areas )
	{
		m_stateCache.SetViewports( *m_pContext, count, areas );
	}

	void D3D11DeferredCommandList::SetScissorRects( uint32 count, const RectangleArea<int32>* areas )
	{
		m_stateCache.SetScissorRects( *m_pContext, count, areas );
	}

	void D3D11DeferredCommandList::BindRenderTargets( aga::Texture** pRenderTargets, uint32 renderTargetCount, aga::Texture* depthStencil )
	{
		m_stateCache.BindRenderTargets( *m_pContext, pRenderTargets, renderTargetCount, depthStencil );
	}

	void D3D11DeferredCommandList::ClearRenderTarget( Texture* renderTarget, const float( &clearColor )[4] )
	{
		ID3D11RenderTargetView* rtvs = nullptr;

		if ( auto rtTex = static_cast<D3D11BaseTexture*>( renderTarget ) )
		{
			if ( auto d3d11RTV = static_cast<D3D11RenderTargetView*>( rtTex->RTV() ) )
			{
				rtvs = d3d11RTV->Resource();
			}
		}

		if ( rtvs == nullptr )
		{
			return;
		}

		m_pContext->ClearRenderTargetView( rtvs, clearColor );
	}

	void D3D11DeferredCommandList::ClearDepthStencil( Texture* depthStencil, float depthColor, UINT8 stencilColor )
	{
		ID3D11DepthStencilView* dsv = nullptr;

		if ( auto dsTex = static_cast<D3D11BaseTexture*>( depthStencil ) )
		{
			if ( auto d3d11DSV = static_cast<D3D11DepthStencilView*>( dsTex->DSV() ) )
			{
				dsv = d3d11DSV->Resource();
			}
		}

		if ( dsv == nullptr )
		{
			return;
		}

		m_pContext->ClearDepthStencilView( dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depthColor, stencilColor );
	}

	void D3D11DeferredCommandList::Finish()
	{
		m_pContext->FinishCommandList( false, &m_pCommandList );
	}

	void D3D11DeferredCommandList::RequestExecute()
	{
		if ( m_pCommandList )
		{
			D3D11Context().ExecuteCommandList( m_pCommandList, false );
			m_pCommandList->Release();
			m_pCommandList = nullptr;
		}
	}

	D3D11DeferredCommandList::D3D11DeferredCommandList()
	{
		[[maybe_unused]] HRESULT hr = D3D11Device().CreateDeferredContext( 0, &m_pContext );
		assert( SUCCEEDED( hr ) );
	}

	D3D11DeferredCommandList::~D3D11DeferredCommandList()
	{
		if ( m_pContext )
		{
			m_pContext->Release();
			m_pContext = nullptr;
		}
	}
}
