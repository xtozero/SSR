#include "D3D12CommandList.h"

#include "D3D12Api.h"
#include "D3D12FlagConvertor.h"
#include "D3D12ResourceViews.h"

using ::Microsoft::WRL::ComPtr;

namespace agl
{
	void D3D12CommandList::Prepare()
	{
	}

	void D3D12CommandList::BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets )
	{
	}

	void D3D12CommandList::BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset )
	{
	}

	void D3D12CommandList::BindPipelineState( GraphicsPipelineState* pipelineState )
	{
	}

	void D3D12CommandList::BindPipelineState( ComputePipelineState* pipelineState )
	{
	}

	void D3D12CommandList::BindShaderResources( ShaderBindings& shaderBindings )
	{
	}

	void D3D12CommandList::SetShaderValue( const ShaderParameter& parameter, const void* value )
	{
	}

	void D3D12CommandList::DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation )
	{
	}

	void D3D12CommandList::DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
	}

	void D3D12CommandList::Dispatch( uint32 x, uint32 y, uint32 z )
	{
	}

	void D3D12CommandList::SetViewports( uint32 count, const CubeArea<float>* area )
	{
	}

	void D3D12CommandList::SetScissorRects( uint32 count, const RectangleArea<int32>* area )
	{
	}

	void D3D12CommandList::BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil )
	{
	}

	void D3D12CommandList::ClearRenderTarget( RenderTargetView* renderTarget, const float( &clearColor )[4] )
	{
	}

	void D3D12CommandList::ClearDepthStencil( DepthStencilView* depthStencil, float depthColor, UINT8 stencilColor )
	{
	}

	void D3D12CommandList::CopyResource( Texture* dest, Texture* src )
	{
	}

	void D3D12CommandList::CopyResource( Buffer* dest, Buffer* src )
	{
	}

	void D3D12CommandList::WaitUntilFlush()
	{
	}

	void D3D12CommandList::Execute( IDeferredCommandList& commandList )
	{
	}

	void D3D12DeferredCommandList::Prepare()
	{
	}

	void D3D12DeferredCommandList::BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets )
	{
	}

	void D3D12DeferredCommandList::BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset )
	{
	}

	void D3D12DeferredCommandList::BindPipelineState( GraphicsPipelineState* pipelineState )
	{
	}

	void D3D12DeferredCommandList::BindPipelineState( ComputePipelineState* pipelineState )
	{
	}

	void D3D12DeferredCommandList::BindShaderResources( ShaderBindings& shaderBindings )
	{
	}

	void D3D12DeferredCommandList::SetShaderValue( const ShaderParameter& parameter, const void* value )
	{
	}

	void D3D12DeferredCommandList::DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation )
	{
	}

	void D3D12DeferredCommandList::DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
	}

	void D3D12DeferredCommandList::Dispatch( uint32 x, uint32 y, uint32 z )
	{
	}

	void D3D12DeferredCommandList::SetViewports( uint32 count, const CubeArea<float>* areas )
	{
	}

	void D3D12DeferredCommandList::SetScissorRects( uint32 count, const RectangleArea<int32>* areas )
	{
	}

	void D3D12DeferredCommandList::BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil )
	{
	}

	void D3D12DeferredCommandList::ClearRenderTarget( RenderTargetView* renderTarget, const float( &clearColor )[4] )
	{
	}

	void D3D12DeferredCommandList::ClearDepthStencil( DepthStencilView* depthStencil, float depthColor, UINT8 stencilColor )
	{
	}

	void D3D12DeferredCommandList::CopyResource( Texture* dest, Texture* src )
	{
	}

	void D3D12DeferredCommandList::CopyResource( Buffer* dest, Buffer* src )
	{
	}

	void D3D12DeferredCommandList::Finish()
	{
	}

	D3D12DeferredCommandList::D3D12DeferredCommandList()
	{
	}

	D3D12DeferredCommandList::~D3D12DeferredCommandList()
	{
	}

	void D3D12GraphicsCommandList::Prepare()
	{
		m_globalConstantBuffers.Prepare();
	}

	void D3D12GraphicsCommandList::BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets )
	{
	}

	void D3D12GraphicsCommandList::BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset )
	{
	}

	void D3D12GraphicsCommandList::BindPipelineState( GraphicsPipelineState* pipelineState )
	{
		m_globalConstantBuffers.Reset( false );
	}

	void D3D12GraphicsCommandList::BindPipelineState( ComputePipelineState* pipelineState )
	{
		m_globalConstantBuffers.Reset( true );
	}

	void D3D12GraphicsCommandList::BindShaderResources( ShaderBindings& shaderBindings )
	{
		m_globalConstantBuffers.AddGlobalConstantBuffers( shaderBindings );
	}

	void D3D12GraphicsCommandList::SetShaderValue( const ShaderParameter& parameter, const void* value )
	{
		m_globalConstantBuffers.SetShaderValue( parameter, value );
	}

	void D3D12GraphicsCommandList::DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation )
	{
		m_globalConstantBuffers.CommitShaderValue( false );
	}

	void D3D12GraphicsCommandList::DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
		m_globalConstantBuffers.CommitShaderValue( false );
	}

	void D3D12GraphicsCommandList::Dispatch( uint32 x, uint32 y, uint32 z )
	{
		m_globalConstantBuffers.CommitShaderValue( true );
	}

	void D3D12GraphicsCommandList::SetViewports( uint32 count, const CubeArea<float>* area )
	{
	}

	void D3D12GraphicsCommandList::SetScissorRects( uint32 count, const RectangleArea<int32>* area )
	{
	}

	void D3D12GraphicsCommandList::BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil )
	{
	}

	void D3D12GraphicsCommandList::ClearRenderTarget( RenderTargetView* renderTarget, const float( &clearColor )[4] )
	{
		if ( renderTarget == nullptr )
		{
			return;
		}

		auto d3d12Rtv = static_cast<D3D12RenderTargetView*>( renderTarget );
		D3D12_CPU_DESCRIPTOR_HANDLE handle = d3d12Rtv->GetCpuHandle().At();
		m_commandList->ClearRenderTargetView( handle, clearColor, 0, nullptr );
	}

	void D3D12GraphicsCommandList::ClearDepthStencil( DepthStencilView* depthStencil, float depthColor, UINT8 stencilColor )
	{
	}

	void D3D12GraphicsCommandList::CopyResource( Texture* dest, Texture* src )
	{
	}

	void D3D12GraphicsCommandList::CopyResource( Buffer* dest, Buffer* src )
	{
	}

	void D3D12GraphicsCommandList::Transition( uint32 numTransitions, const ResourceTransition* transitions )
	{
		std::vector<D3D12_RESOURCE_BARRIER, InlineAllocator<D3D12_RESOURCE_BARRIER, 1>> d3d12Barriers;
		d3d12Barriers.resize( numTransitions );
		for ( uint32 i = 0; i < numTransitions; ++i )
		{
			d3d12Barriers[i] = ConvertToResourceBarrier( transitions[i] );
		}

		m_commandList->ResourceBarrier( numTransitions, d3d12Barriers.data());
	}

	void D3D12GraphicsCommandList::Initialize()
	{
		m_globalConstantBuffers.Initialize();

		[[maybe_unused]] HRESULT hr = D3D12Device().CreateCommandAllocator( D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( &m_commandAllocator ) );
		assert( SUCCEEDED( hr ) );

		ComPtr<ID3D12GraphicsCommandList> commandList;

		hr = D3D12Device().CreateCommandList( 0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS( &commandList ) );
		assert( SUCCEEDED( hr ) );

		commandList.As( &m_commandList );
	}

	void D3D12GraphicsCommandList::Reset()
	{
		if ( ( m_commandAllocator != nullptr ) && ( m_commandList != nullptr ) )
		{
			m_commandAllocator->Reset();
			m_commandList->Reset( m_commandAllocator.Get(), nullptr );
		}
	}

	void D3D12GraphicsCommandList::Close()
	{
		if ( m_commandList != nullptr )
		{
			m_commandList->Close();
		}
	}

	ID3D12CommandList* D3D12GraphicsCommandList::Resource()
	{
		return m_commandList.Get();
	}

	Owner<IGraphicsCommandList*> D3D12GraphicsCommandLists::CreateCommandList()
	{
		D3D12GraphicsCommandList* commandList = new D3D12GraphicsCommandList();
		commandList->Initialize();
		return commandList;
	}
	
	void D3D12GraphicsCommandLists::Prepare()
	{
		for ( auto commandList : m_commandLists )
		{
			auto d3d12CommandList = static_cast<D3D12GraphicsCommandList*>( commandList );
			d3d12CommandList->Prepare();
			d3d12CommandList->Reset();
		}
	}

	void D3D12GraphicsCommandLists::Commit()
	{
		std::vector<ID3D12CommandList*, InlineAllocator<ID3D12CommandList*, 1>> commandLists;
		for ( auto commandList : m_commandLists )
		{
			auto d3d12CommandList = static_cast<D3D12GraphicsCommandList*>( commandList );
			d3d12CommandList->Close();

			commandLists.push_back( d3d12CommandList->Resource() );
		}

		auto numCommandList = static_cast<uint32>( commandLists.size() );
		D3D12DirectCommandQueue().ExecuteCommandLists( numCommandList, commandLists.data() );
	}
}
