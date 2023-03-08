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
	void D3D11CommandList::Prepare()
	{
		m_globalConstantBuffers.Prepare();

		for ( auto commandList : m_parallelCommandLists )
		{
			auto d3d12CommandList = static_cast<D3D11ParallelCommandList*>( commandList );
			d3d12CommandList->Prepare();
		}

		m_numUsedParallelCommandList = 0;
	}

	void D3D11CommandList::BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets )
	{
		m_stateCache.BindVertexBuffer( D3D11Context(), vertexBuffers, startSlot, numBuffers, pOffsets );
	}

	void D3D11CommandList::BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset )
	{
		m_stateCache.BindIndexBuffer( D3D11Context(), indexBuffer, indexOffset );
	}

	void D3D11CommandList::BindPipelineState( GraphicsPipelineState* pipelineState )
	{
		m_globalConstantBuffers.Reset( false );
		m_stateCache.BindPipelineState( D3D11Context(), pipelineState );
	}

	void D3D11CommandList::BindPipelineState( ComputePipelineState* pipelineState )
	{
		m_globalConstantBuffers.Reset( true );
		m_stateCache.BindPipelineState( D3D11Context(), pipelineState );
	}

	void D3D11CommandList::BindShaderResources( ShaderBindings& shaderBindings )
	{
		m_globalConstantBuffers.AddGlobalConstantBuffers( shaderBindings );
		m_stateCache.BindShaderResources( D3D11Context(), shaderBindings );
	}

	void D3D11CommandList::SetShaderValue( const ShaderParameter& parameter, const void* value )
	{
		m_globalConstantBuffers.SetShaderValue( parameter, value );
	}

	void D3D11CommandList::DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation )
	{
		m_globalConstantBuffers.CommitShaderValue( false );
		D3D11Context().DrawInstanced( vertexCount, numInstance, baseVertexLocation, 0 );
	}

	void D3D11CommandList::DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
		m_globalConstantBuffers.CommitShaderValue( false );
		D3D11Context().DrawIndexedInstanced( indexCount, numInstance, startIndexLocation, baseVertexLocation, 0 );
	}

	void D3D11CommandList::Dispatch( uint32 x, uint32 y, uint32 z )
	{
		m_globalConstantBuffers.CommitShaderValue( true );
		D3D11Context().Dispatch( x, y, z );
	}

	void D3D11CommandList::SetViewports( uint32 count, const CubeArea<float>* areas )
	{
		m_stateCache.SetViewports( D3D11Context(), count, areas );
	}

	void D3D11CommandList::SetScissorRects( uint32 count, const RectangleArea<int32>* areas )
	{
		m_stateCache.SetScissorRects( D3D11Context(), count, areas );
	}

	void D3D11CommandList::BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil )
	{
		m_stateCache.BindRenderTargets( D3D11Context(), pRenderTargets, renderTargetCount, depthStencil );
	}

	void D3D11CommandList::ClearRenderTarget( RenderTargetView* renderTarget, const float( &clearColor )[4] )
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

	void D3D11CommandList::ClearDepthStencil( DepthStencilView* depthStencil, float depthColor, UINT8 stencilColor )
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

	void D3D11CommandList::CopyResource( Texture* dest, Texture* src )
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

	void D3D11CommandList::CopyResource( Buffer* dest, Buffer* src )
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

	void D3D11CommandList::Transition( [[maybe_unused]] uint32 numTransitions, [[maybe_unused]] const ResourceTransition* transitions )
	{
		// Do Nothing
	}

	void D3D11CommandList::WaitUntilFlush()
	{
		D3D11Context().Flush();
	}

	void D3D11CommandList::Commit()
	{
		for ( size_t i = 0; i < m_parallelCommandLists.size(); ++i )
		{
			auto d3d12CommandList = static_cast<D3D11ParallelCommandList*>( m_parallelCommandLists[i] );
			d3d12CommandList->Close();

			d3d12CommandList->Commit();
		}
	}
	
	void D3D11CommandList::Initialize()
	{
		m_globalConstantBuffers.Initialize();
	}

	IParallelCommandList& D3D11CommandList::GetParallelCommandList()
	{
		if ( m_numUsedParallelCommandList >= m_parallelCommandLists.size() )
		{
			auto newCommandList = new D3D11ParallelCommandList();
			newCommandList->Initialize();
			newCommandList->Prepare();

			m_parallelCommandLists.push_back( newCommandList );
		}

		return *m_parallelCommandLists[m_numUsedParallelCommandList++];
	}

	D3D11CommandList::D3D11CommandList( D3D11CommandList&& other ) noexcept
	{
		*this = std::move( other );
	}

	D3D11CommandList& D3D11CommandList::operator=( D3D11CommandList&& other ) noexcept
	{
		if ( this != &other )
		{
			m_stateCache = std::move( other.m_stateCache );
			m_globalConstantBuffers = std::move( other.m_globalConstantBuffers );
			m_numUsedParallelCommandList = other.m_numUsedParallelCommandList;
			m_parallelCommandLists = std::move( other.m_parallelCommandLists );
		}

		return *this;
	}

	D3D11CommandList::~D3D11CommandList()
	{
		for ( IParallelCommandList* commandList : m_parallelCommandLists )
		{
			delete commandList;
		}
		m_parallelCommandLists.clear();
	}

	void D3D11ParallelCommandList::Prepare()
	{
		m_globalConstantBuffers.Prepare();
	}

	void D3D11ParallelCommandList::BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets )
	{
		m_stateCache.BindVertexBuffer( *m_pContext.Get(), vertexBuffers, startSlot, numBuffers, pOffsets);
	}

	void D3D11ParallelCommandList::BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset )
	{
		m_stateCache.BindIndexBuffer( *m_pContext.Get(), indexBuffer, indexOffset );
	}

	void D3D11ParallelCommandList::BindPipelineState( GraphicsPipelineState* pipelineState )
	{
		m_globalConstantBuffers.Reset( false );
		m_stateCache.BindPipelineState( *m_pContext.Get(), pipelineState );
	}

	void D3D11ParallelCommandList::BindPipelineState( ComputePipelineState* pipelineState )
	{
		m_globalConstantBuffers.Reset( true );
		m_stateCache.BindPipelineState( *m_pContext.Get(), pipelineState );
	}

	void D3D11ParallelCommandList::BindShaderResources( ShaderBindings& shaderBindings )
	{
		m_globalConstantBuffers.AddGlobalConstantBuffers( shaderBindings );
		m_stateCache.BindShaderResources( *m_pContext.Get(), shaderBindings );
	}

	void D3D11ParallelCommandList::SetShaderValue( const ShaderParameter& parameter, const void* value )
	{
		m_globalConstantBuffers.SetShaderValue( parameter, value );
	}

	void D3D11ParallelCommandList::DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation )
	{
		m_globalConstantBuffers.CommitShaderValue( false );
		m_pContext->DrawInstanced( vertexCount, numInstance, baseVertexLocation, 0 );
	}

	void D3D11ParallelCommandList::DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
		m_globalConstantBuffers.CommitShaderValue( false );
		m_pContext->DrawIndexedInstanced( indexCount, numInstance, startIndexLocation, baseVertexLocation, 0 );
	}

	void D3D11ParallelCommandList::Dispatch( uint32 x, uint32 y, uint32 z )
	{
		m_globalConstantBuffers.CommitShaderValue( true );
		m_pContext->Dispatch( x, y, z );
	}

	void D3D11ParallelCommandList::SetViewports( uint32 count, const CubeArea<float>* areas )
	{
		m_stateCache.SetViewports( *m_pContext.Get(), count, areas );
	}

	void D3D11ParallelCommandList::SetScissorRects( uint32 count, const RectangleArea<int32>* areas )
	{
		m_stateCache.SetScissorRects( *m_pContext.Get(), count, areas );
	}

	void D3D11ParallelCommandList::BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil )
	{
		m_stateCache.BindRenderTargets( *m_pContext.Get(), pRenderTargets, renderTargetCount, depthStencil );
	}

	void D3D11ParallelCommandList::ClearRenderTarget( RenderTargetView* renderTarget, const float( &clearColor )[4] )
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

	void D3D11ParallelCommandList::ClearDepthStencil( DepthStencilView* depthStencil, float depthColor, UINT8 stencilColor )
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

	void D3D11ParallelCommandList::CopyResource( Texture* dest, Texture* src )
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

	void D3D11ParallelCommandList::CopyResource( Buffer* dest, Buffer* src )
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

	void D3D11ParallelCommandList::Transition( [[maybe_unused]] uint32 numTransitions, [[maybe_unused]] const ResourceTransition* transitions )
	{
		// Do Nothing
	}

	void D3D11ParallelCommandList::Close()
	{
		m_pContext->FinishCommandList( false, m_pCommandList.GetAddressOf() );
	}

	void D3D11ParallelCommandList::Commit()
	{
		if ( m_pCommandList )
		{
			D3D11Context().ExecuteCommandList( m_pCommandList.Get(), false);
			m_pCommandList = nullptr;
		}
	}

	void D3D11ParallelCommandList::Initialize()
	{
		[[maybe_unused]] HRESULT hr = D3D11Device().CreateDeferredContext( 0, &m_pContext );
		assert( SUCCEEDED( hr ) );

		m_globalConstantBuffers.Initialize();
	}
}
