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

namespace agl
{
	void D3D11ImmediateCommandList::Prepare()
	{
		m_globalConstantBuffers.Prepare();
	}

	void D3D11ImmediateCommandList::BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets )
	{
		m_stateCache.BindVertexBuffer( D3D11Context(), vertexBuffers, startSlot, numBuffers, pOffsets );
	}

	void D3D11ImmediateCommandList::BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset )
	{
		m_stateCache.BindIndexBuffer( D3D11Context(), indexBuffer, indexOffset );
	}

	void D3D11ImmediateCommandList::BindPipelineState( GraphicsPipelineState* pipelineState )
	{
		m_globalConstantBuffers.Reset( false );
		m_stateCache.BindPipelineState( D3D11Context(), pipelineState );
	}

	void D3D11ImmediateCommandList::BindPipelineState( ComputePipelineState* pipelineState )
	{
		m_globalConstantBuffers.Reset( true );
		m_stateCache.BindPipelineState( D3D11Context(), pipelineState );
	}

	void D3D11ImmediateCommandList::BindShaderResources( ShaderBindings& shaderBindings )
	{
		m_globalConstantBuffers.AddGlobalConstantBuffers( shaderBindings );
		m_stateCache.BindShaderResources( D3D11Context(), shaderBindings );
	}

	void D3D11ImmediateCommandList::SetShaderValue( const ShaderParameter& parameter, const void* value )
	{
		m_globalConstantBuffers.SetShaderValue( parameter, value );
	}

	void D3D11ImmediateCommandList::DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation )
	{
		m_globalConstantBuffers.CommitShaderValue( false );
		D3D11Context().DrawInstanced( vertexCount, numInstance, baseVertexLocation, 0 );
	}

	void D3D11ImmediateCommandList::DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
		m_globalConstantBuffers.CommitShaderValue( false );
		D3D11Context().DrawIndexedInstanced( indexCount, numInstance, startIndexLocation, baseVertexLocation, 0 );
	}

	void D3D11ImmediateCommandList::Dispatch( uint32 x, uint32 y, uint32 z )
	{
		m_globalConstantBuffers.CommitShaderValue( true );
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

	void D3D11ImmediateCommandList::BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil )
	{
		m_stateCache.BindRenderTargets( D3D11Context(), pRenderTargets, renderTargetCount, depthStencil );
	}

	void D3D11ImmediateCommandList::ClearRenderTarget( RenderTargetView* renderTarget, const float( &clearColor )[4] )
	{
		ID3D11RenderTargetView* rtvs = nullptr;

		if ( auto d3d11RTV = static_cast<D3D11RenderTargetView*>( renderTarget ) )
		{
			rtvs = d3d11RTV->Resource();
		}

		if ( rtvs == nullptr )
		{
			return;
		}

		D3D11Context().ClearRenderTargetView( rtvs, clearColor );
	}

	void D3D11ImmediateCommandList::ClearDepthStencil( DepthStencilView* depthStencil, float depthColor, UINT8 stencilColor )
	{
		ID3D11DepthStencilView* dsv = nullptr;

		if ( auto d3d11DSV = static_cast<D3D11DepthStencilView*>( depthStencil ) )
		{
			dsv = d3d11DSV->Resource();
		}

		if ( dsv == nullptr )
		{
			return;
		}

		D3D11Context().ClearDepthStencilView( dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depthColor, stencilColor );
	}

	void D3D11ImmediateCommandList::CopyResource( Texture* dest, Texture* src )
	{
		if ( dest == nullptr || src == nullptr )
		{
			assert( "Can't copy with null resource" );
			return;
		}

		ID3D11Resource* destResource = nullptr;
		ID3D11Resource* srcResource = nullptr;

		if ( auto destTexture = static_cast<TextureBase*>( dest ) )
		{
			destResource = static_cast<ID3D11Resource*>( destTexture->Resource() );
		}

		if ( auto srcTexture = static_cast<TextureBase*>( src ) )
		{
			srcResource = static_cast<ID3D11Resource*>( srcTexture->Resource() );
		}

		D3D11Context().CopyResource( destResource, srcResource );
	}

	void D3D11ImmediateCommandList::CopyResource( Buffer* dest, Buffer* src )
	{
		if ( dest == nullptr || src == nullptr )
		{
			assert( "Can't copy with null resource" );
			return;
		}

		ID3D11Resource* destResource = nullptr;
		ID3D11Resource* srcResource = nullptr;

		if ( auto destBuffer = static_cast<D3D11Buffer*>( dest ) )
		{
			destResource = destBuffer->Resource();
		}

		if ( auto srcBuffer = static_cast<D3D11Buffer*>( src ) )
		{
			srcResource = srcBuffer->Resource();
		}

		D3D11Context().CopyResource( destResource, srcResource );
	}

	void D3D11ImmediateCommandList::WaitUntilFlush()
	{
		D3D11Context().Flush();
	}

	void D3D11ImmediateCommandList::Execute( IDeferredCommandList& commandList )
	{
		D3D11DeferredCommandList& d3d11DeferredCommandList = static_cast<D3D11DeferredCommandList&>( commandList );
		d3d11DeferredCommandList.RequestExecute();
	}

	D3D11ImmediateCommandList::D3D11ImmediateCommandList()
	{
		m_globalConstantBuffers.Initialize();
	}

	void D3D11DeferredCommandList::Prepare()
	{
		m_globalConstantBuffers.Prepare();
	}

	void D3D11DeferredCommandList::BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets )
	{
		m_stateCache.BindVertexBuffer( *m_pContext, vertexBuffers, startSlot, numBuffers, pOffsets );
	}

	void D3D11DeferredCommandList::BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset )
	{
		m_stateCache.BindIndexBuffer( *m_pContext, indexBuffer, indexOffset );
	}

	void D3D11DeferredCommandList::BindPipelineState( GraphicsPipelineState* pipelineState )
	{
		m_globalConstantBuffers.Reset( false );
		m_stateCache.BindPipelineState( *m_pContext, pipelineState );
	}

	void D3D11DeferredCommandList::BindPipelineState( ComputePipelineState* pipelineState )
	{
		m_globalConstantBuffers.Reset( true );
		m_stateCache.BindPipelineState( *m_pContext, pipelineState );
	}

	void D3D11DeferredCommandList::BindShaderResources( ShaderBindings& shaderBindings )
	{
		m_stateCache.BindShaderResources( *m_pContext, shaderBindings );
	}

	void D3D11DeferredCommandList::SetShaderValue( const ShaderParameter& parameter, const void* value )
	{
		m_globalConstantBuffers.SetShaderValue( parameter, value );
	}

	void D3D11DeferredCommandList::DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation )
	{
		m_globalConstantBuffers.CommitShaderValue( false );
		m_pContext->DrawInstanced( vertexCount, numInstance, baseVertexLocation, 0 );
	}

	void D3D11DeferredCommandList::DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
		m_globalConstantBuffers.CommitShaderValue( false );
		m_pContext->DrawIndexedInstanced( indexCount, numInstance, startIndexLocation, baseVertexLocation, 0 );
	}

	void D3D11DeferredCommandList::Dispatch( uint32 x, uint32 y, uint32 z )
	{
		m_globalConstantBuffers.CommitShaderValue( true );
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

	void D3D11DeferredCommandList::BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil )
	{
		m_stateCache.BindRenderTargets( *m_pContext, pRenderTargets, renderTargetCount, depthStencil );
	}

	void D3D11DeferredCommandList::ClearRenderTarget( RenderTargetView* renderTarget, const float( &clearColor )[4] )
	{
		ID3D11RenderTargetView* rtvs = nullptr;

		if ( auto d3d11RTV = static_cast<D3D11RenderTargetView*>( renderTarget ) )
		{
			rtvs = d3d11RTV->Resource();
		}

		if ( rtvs == nullptr )
		{
			return;
		}

		m_pContext->ClearRenderTargetView( rtvs, clearColor );
	}

	void D3D11DeferredCommandList::ClearDepthStencil( DepthStencilView* depthStencil, float depthColor, UINT8 stencilColor )
	{
		ID3D11DepthStencilView* dsv = nullptr;

		if ( auto d3d11DSV = static_cast<D3D11DepthStencilView*>( depthStencil ) )
		{
			dsv = d3d11DSV->Resource();
		}

		if ( dsv == nullptr )
		{
			return;
		}

		m_pContext->ClearDepthStencilView( dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depthColor, stencilColor );
	}

	void D3D11DeferredCommandList::CopyResource( Texture* dest, Texture* src )
	{
		if ( dest == nullptr || src == nullptr )
		{
			assert( "Can't copy with null resource" );
			return;
		}

		ID3D11Resource* destResource = nullptr;
		ID3D11Resource* srcResource = nullptr;

		if ( auto destTexture = static_cast<TextureBase*>( dest ) )
		{
			destResource = static_cast<ID3D11Resource*>( destTexture->Resource() );
		}

		if ( auto srcTexture = static_cast<TextureBase*>( src ) )
		{
			srcResource = static_cast<ID3D11Resource*>( srcTexture->Resource() );
		}

		m_pContext->CopyResource( destResource, srcResource );
	}

	void D3D11DeferredCommandList::CopyResource( Buffer* dest, Buffer* src )
	{
		if ( dest == nullptr || src == nullptr )
		{
			assert( "Can't copy with null resource" );
			return;
		}

		ID3D11Resource* destResource = nullptr;
		ID3D11Resource* srcResource = nullptr;

		if ( auto destBuffer = static_cast<D3D11Buffer*>( dest ) )
		{
			destResource = destBuffer->Resource();
		}

		if ( auto srcBuffer = static_cast<D3D11Buffer*>( src ) )
		{
			srcResource = srcBuffer->Resource();
		}

		m_pContext->CopyResource( destResource, srcResource );
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

		m_globalConstantBuffers.Initialize();
	}

	D3D11DeferredCommandList::~D3D11DeferredCommandList()
	{
		if ( m_pContext )
		{
			m_pContext->Release();
			m_pContext = nullptr;
		}
	}

	Owner<IGraphicsCommandList*> D3D11GraphicsCommandLists::CreateCommandList()
	{
		return nullptr;
	}

	void D3D11GraphicsCommandLists::Prepare()
	{
	}

	void D3D11GraphicsCommandLists::Commit()
	{
	}
}
