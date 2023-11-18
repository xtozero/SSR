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

namespace
{
	using ::agl::Buffer;
	using ::agl::BufferTrait;
	using ::agl::D3D11Buffer;
	using ::agl::RefHandle;
	using ::agl::ResourceAccessFlag;
	using ::agl::ResourceBindType;
	using ::agl::ResourceMisc;
	using ::agl::Texture;
	using ::agl::TextureBase;
	using ::agl::TextureTrait;

	void UpdateSubresource( ID3D11DeviceContext& context, agl::Buffer* dest, const void* src, uint32 destOffset, uint32 numByte )
	{
		auto destBuffer = static_cast<D3D11Buffer*>( dest );
		if ( destBuffer == nullptr )
		{
			return;
		}

		const BufferTrait& destTrait = destBuffer->GetTrait();
		if ( numByte == 0 )
		{
			numByte = destTrait.m_stride * destTrait.m_count;
		}

		BufferTrait intermediateTrait = {
			.m_stride = numByte,
			.m_count = 1,
			.m_access = ResourceAccessFlag::Upload,
			.m_bindType = ResourceBindType::None,
			.m_miscFlag = ResourceMisc::Intermediate,
			.m_format = destTrait.m_format
		};

		RefHandle<D3D11Buffer> intermediate = static_cast<D3D11Buffer*>( Buffer::Create( intermediateTrait, "UpdateSubresource.Intermediate" ).Get() );
		intermediate->Init();

		D3D11_MAPPED_SUBRESOURCE lockedResource = {};
		[[maybe_unused]] HRESULT hr = context.Map( static_cast<ID3D11Resource*>( intermediate->Resource() ), 0, D3D11_MAP_WRITE_DISCARD, 0, &lockedResource );
		assert( SUCCEEDED( hr ) );

		std::memcpy( lockedResource.pData, src, numByte );

		context.Unmap( static_cast<ID3D11Resource*>( intermediate->Resource() ), 0 );

		D3D11_BOX srcRange = {
			.left = 0,
			.top = 0,
			.front = 0,
			.right = numByte,
			.bottom = 1,
			.back = 1
		};

		context.CopySubresourceRegion(
			destBuffer->Resource(),
			0,
			destOffset,
			0,
			0,
			intermediate->Resource(),
			0,
			&srcRange
		);
	}

	void UpdateSubresource( ID3D11DeviceContext& context, agl::Texture* dest, const void* src, uint32 srcRowSize, const CubeArea<uint32>* pDestArea, uint32 subresource )
	{
		auto destTexture = static_cast<TextureBase*>( dest );
		if ( destTexture == nullptr )
		{
			return;
		}

		const TextureTrait& destTrait = destTexture->GetTrait();
		bool bIsTexture3D = HasAnyFlags( destTrait.m_miscFlag, ResourceMisc::Texture3D );

		CubeArea<uint32> destArea;
		if ( pDestArea == nullptr )
		{
			destArea = {
				.m_left = 0,
				.m_top = 0,
				.m_front = 0,
				.m_right = destTrait.m_width,
				.m_bottom = destTrait.m_height,
				.m_back = bIsTexture3D ? destTrait.m_depth : 1,
			};
		}
		else
		{
			destArea = *pDestArea;
		}

		TextureTrait intermediateTrait = {
			.m_width = destArea.m_right - destArea.m_left,
			.m_height = destArea.m_bottom - destArea.m_top,
			.m_depth = bIsTexture3D ? ( destArea.m_back - destArea.m_front ) : 1,
			.m_sampleCount = destTrait.m_sampleCount,
			.m_sampleQuality = destTrait.m_sampleQuality,
			.m_mipLevels = 1,
			.m_format = destTrait.m_format,
			.m_access = ResourceAccessFlag::Upload,
			.m_bindType = ResourceBindType::None,
			.m_miscFlag = bIsTexture3D ? ( ResourceMisc::Texture3D | ResourceMisc::Intermediate ) : ResourceMisc::Intermediate
		};

		RefHandle<TextureBase> intermediate = static_cast<TextureBase*>( Texture::Create( intermediateTrait, "UpdateSubresource.Intermediate" ).Get() );
		intermediate->Init();

		D3D11_MAPPED_SUBRESOURCE lockedResource = {};
		[[maybe_unused]] HRESULT hr = context.Map( static_cast<ID3D11Resource*>( intermediate->Resource() ), 0, D3D11_MAP_WRITE_DISCARD, 0, &lockedResource );
		assert( SUCCEEDED( hr ) );

		auto srcData = static_cast<const uint8*>( src );
		auto destData = static_cast<uint8*>( lockedResource.pData );

		for ( uint32 z = 0; z < destTrait.m_depth; ++z )
		{
			uint8* row = destData;

			for ( uint32 y = 0; y < destTrait.m_height; ++y )
			{
				std::memcpy( row, srcData, srcRowSize );
				srcData += srcRowSize;
				row += lockedResource.RowPitch;
			}

			destData += lockedResource.DepthPitch;
		}

		context.Unmap( static_cast<ID3D11Resource*>( intermediate->Resource() ), 0 );

		D3D11_BOX srcRange = {
			.left = 0,
			.top = 0,
			.front = 0,
			.right = intermediateTrait.m_width,
			.bottom = intermediateTrait.m_height,
			.back = intermediateTrait.m_depth
		};

		context.CopySubresourceRegion(
			static_cast<ID3D11Resource*>( destTexture->Resource() ),
			subresource,
			destArea.m_left,
			destArea.m_top,
			destArea.m_front,
			static_cast<ID3D11Resource*>( intermediate->Resource() ),
			0,
			&srcRange
		);
	}
}

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
		m_stateCache.BindPipelineState( D3D11Context(), pipelineState );
	}

	void D3D11CommandList::BindPipelineState( ComputePipelineState* pipelineState )
	{
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
		m_globalConstantBuffers.Reset( false );
	}

	void D3D11CommandList::DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
		m_globalConstantBuffers.CommitShaderValue( false );
		D3D11Context().DrawIndexedInstanced( indexCount, numInstance, startIndexLocation, baseVertexLocation, 0 );
		m_globalConstantBuffers.Reset( false );
	}

	void D3D11CommandList::Dispatch( uint32 x, uint32 y, uint32 z )
	{
		m_globalConstantBuffers.CommitShaderValue( true );
		D3D11Context().Dispatch( x, y, z );
		m_globalConstantBuffers.Reset( true );
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

	void D3D11CommandList::CopyResource( Texture* dest, Texture* src, [[maybe_unused]] bool bDirect )
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

	void D3D11CommandList::CopyResource( Buffer* dest, Buffer* src, uint32 numByte, [[maybe_unused]] bool bDirect )
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

		const BufferTrait& destTrait = dest->GetTrait();
		const BufferTrait& srcTrait = src->GetTrait();

		if ( ( numByte == 0 )
			|| ( ( destTrait.m_stride * destTrait.m_count ) == ( srcTrait.m_stride * srcTrait.m_count ) ) )
		{
			D3D11Context().CopyResource( destResource, srcResource );
		}
		else
		{
			D3D11_BOX srcBox = {
				.left = 0,
				.top = 0,
				.front = 0,
				.right = numByte,
				.bottom = 1,
				.back = 1,
			};

			D3D11Context().CopySubresourceRegion(destResource, 0, 0, 0, 0, srcResource, 0, &srcBox );
		}
	}

	void D3D11CommandList::UpdateSubresource( agl::Texture* dest, const void* src, uint32 srcRowSize, const CubeArea<uint32>* destArea, uint32 subresource )
	{
		::UpdateSubresource( D3D11Context(), dest, src, srcRowSize, destArea, subresource );
	}

	void D3D11CommandList::UpdateSubresource( agl::Buffer* dest, const void* src, uint32 destOffset, uint32 numByte )
	{
		::UpdateSubresource( D3D11Context(), dest, src, destOffset, numByte );
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

	void D3D11ParallelCommandList::CopyResource( Texture* dest, Texture* src, [[maybe_unused]] bool bDirect )
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

	void D3D11ParallelCommandList::CopyResource( Buffer* dest, Buffer* src, uint32 numByte, [[maybe_unused]] bool bDirect )
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

		const BufferTrait& destTrait = dest->GetTrait();
		const BufferTrait& srcTrait = src->GetTrait();

		if ( ( numByte == 0 )
			|| ( ( destTrait.m_stride * destTrait.m_stride ) == ( srcTrait.m_stride * srcTrait.m_stride ) ) )
		{
			m_pContext->CopyResource( destResource, srcResource );
		}
		else
		{
			D3D11_BOX srcBox = {
				.left = 0,
				.top = 0,
				.front = 0,
				.right = numByte,
				.bottom = 1,
				.back = 1,
			};

			m_pContext->CopySubresourceRegion( destResource, 0, 0, 0, 0, srcResource, 0, &srcBox );
		}
	}

	void D3D11ParallelCommandList::UpdateSubresource( agl::Texture* dest, const void* src, uint32 srcRowSize, const CubeArea<uint32>* destArea, uint32 subresource )
	{
		::UpdateSubresource( *m_pContext.Get(), dest, src, srcRowSize, destArea, subresource);
	}

	void D3D11ParallelCommandList::UpdateSubresource( agl::Buffer* dest, const void* src, uint32 destOffset, uint32 numByte )
	{
		::UpdateSubresource( *m_pContext.Get(), dest, src, destOffset, numByte );
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
