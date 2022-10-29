#include "D3D12CommandList.h"

namespace agl
{
	void D3D12CommandList::BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets )
	{
	}

	void D3D12CommandList::BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset )
	{
	}

	void D3D12CommandList::BindPipelineState( PipelineState* pipelineState )
	{
	}

	void D3D12CommandList::BindShader( VertexShader* vs )
	{
	}

	void D3D12CommandList::BindShader( GeometryShader* gs )
	{
	}

	void D3D12CommandList::BindShader( PixelShader* ps )
	{
	}

	void D3D12CommandList::BindShader( ComputeShader* cs )
	{
	}

	void D3D12CommandList::BindShaderResources( const ShaderBindings& shaderBindings )
	{
	}

	void D3D12CommandList::BindConstantBuffer( ShaderType shader, uint32 slot, Buffer* buffer )
	{
	}

	void D3D12CommandList::BindSRV( ShaderType shader, uint32 slot, ShaderResourceView* srv )
	{
	}

	void D3D12CommandList::BindUAV( ShaderType shader, uint32 slot, UnorderedAccessView* uav )
	{
	}

	void D3D12CommandList::BindSampler( ShaderType shader, uint32 slot, SamplerState* sampler )
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

	void D3D12DeferredCommandList::BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets )
	{
	}

	void D3D12DeferredCommandList::BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset )
	{
	}

	void D3D12DeferredCommandList::BindPipelineState( PipelineState* pipelineState )
	{
	}

	void D3D12DeferredCommandList::BindShader( VertexShader* vs )
	{
	}

	void D3D12DeferredCommandList::BindShader( GeometryShader* gs )
	{
	}

	void D3D12DeferredCommandList::BindShader( PixelShader* ps )
	{
	}

	void D3D12DeferredCommandList::BindShader( ComputeShader* cs )
	{
	}

	void D3D12DeferredCommandList::BindShaderResources( const ShaderBindings& shaderBindings )
	{
	}

	void D3D12DeferredCommandList::BindConstantBuffer( ShaderType shader, uint32 slot, Buffer* buffer )
	{
	}

	void D3D12DeferredCommandList::BindSRV( ShaderType shader, uint32 slot, ShaderResourceView* srv )
	{
	}

	void D3D12DeferredCommandList::BindUAV( ShaderType shader, uint32 slot, UnorderedAccessView* uav )
	{
	}

	void D3D12DeferredCommandList::BindSampler( ShaderType shader, uint32 slot, SamplerState* sampler )
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
}
