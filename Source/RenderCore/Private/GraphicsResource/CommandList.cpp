#include "CommandList.h"

#include "AbstractGraphicsInterface.h"
#include "InterfaceFactories.h"

namespace rendercore
{
	void ImmediateCommandList::BindVertexBuffer( agl::Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets )
	{
		m_imple.BindVertexBuffer( vertexBuffers, startSlot, numBuffers, pOffsets );
	}

	void ImmediateCommandList::BindIndexBuffer( agl::Buffer* indexBuffer, uint32 indexOffset )
	{
		m_imple.BindIndexBuffer( indexBuffer, indexOffset );
	}

	void ImmediateCommandList::BindPipelineState( agl::GraphicsPipelineState* pipelineState )
	{
		m_imple.BindPipelineState( pipelineState );
	}

	void ImmediateCommandList::BindPipelineState( agl::ComputePipelineState* pipelineState )
	{
		m_imple.BindPipelineState( pipelineState );
	}

	void ImmediateCommandList::BindShaderResources( agl::ShaderBindings& shaderBindings )
	{
		m_imple.BindShaderResources( shaderBindings );
	}

	void ImmediateCommandList::SetShaderValue( const agl::ShaderParameter& parameter, const void* value )
	{
		m_imple.SetShaderValue( parameter, value );
	}

	void ImmediateCommandList::DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation )
	{
		m_imple.DrawInstanced( vertexCount, numInstance, baseVertexLocation );
	}

	void ImmediateCommandList::DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
		m_imple.DrawIndexedInstanced( indexCount, numInstance, startIndexLocation, baseVertexLocation );
	}

	void ImmediateCommandList::Dispatch( uint32 x, uint32 y, uint32 z )
	{
		m_imple.Dispatch( x, y, z );
		BindPipelineState( static_cast<agl::ComputePipelineState*>( nullptr ) );
	}

	void ImmediateCommandList::SetViewports( uint32 count, const agl::CubeArea<float>* areas )
	{
		m_imple.SetViewports( count, areas );
	}

	void ImmediateCommandList::SetScissorRects( uint32 count, const agl::RectangleArea<int32>* areas )
	{
		m_imple.SetScissorRects( count, areas );
	}

	void ImmediateCommandList::BindRenderTargets( agl::RenderTargetView** pRenderTargets, uint32 renderTargetCount, agl::DepthStencilView* depthStencil )
	{
		m_imple.BindRenderTargets( pRenderTargets, renderTargetCount, depthStencil );
	}

	void ImmediateCommandList::ClearRenderTarget( agl::RenderTargetView* renderTarget, const float( &clearColor )[4] )
	{
		m_imple.ClearRenderTarget( renderTarget, clearColor );
	}

	void ImmediateCommandList::ClearDepthStencil( agl::DepthStencilView* depthStencil, float depthColor, UINT8 stencilColor )
	{
		m_imple.ClearDepthStencil( depthStencil, depthColor, stencilColor );
	}

	void ImmediateCommandList::CopyResource( agl::Texture* dest, agl::Texture* src )
	{
		m_imple.CopyResource( dest, src );
	}

	void ImmediateCommandList::CopyResource( agl::Buffer* dest, agl::Buffer* src )
	{
		m_imple.CopyResource( dest, src );
	}

	void ImmediateCommandList::WaitUntilFlush()
	{
		m_imple.WaitUntilFlush();
	}

	void ImmediateCommandList::Execute( agl::IDeferredCommandList& commandList )
	{
		m_imple.Execute( commandList );
	}

	ImmediateCommandList GetImmediateCommandList()
	{
		auto commandList = GraphicsInterface().GetImmediateCommandList();
		return ImmediateCommandList( *commandList );
	}
}
