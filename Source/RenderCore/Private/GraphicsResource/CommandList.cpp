#include "CommandList.h"

#include "AbstractGraphicsInterface.h"
#include "InterfaceFactories.h"
#include "Query.h"

namespace rendercore
{
	void CommandList::BindVertexBuffer( agl::Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* strides, const uint32* pOffsets )
	{
		m_imple.BindVertexBuffer( vertexBuffers, startSlot, numBuffers, strides, pOffsets );
	}

	void CommandList::BindIndexBuffer( agl::Buffer* indexBuffer, uint32 indexOffset )
	{
		m_imple.BindIndexBuffer( indexBuffer, indexOffset );
	}

	void CommandList::BindPipelineState( agl::GraphicsPipelineState* pipelineState )
	{
		m_imple.BindPipelineState( pipelineState );
	}

	void CommandList::BindPipelineState( agl::ComputePipelineState* pipelineState )
	{
		m_imple.BindPipelineState( pipelineState );
	}

	void CommandList::BindShaderResources( agl::ShaderBindings& shaderBindings )
	{
		m_imple.BindShaderResources( shaderBindings );
	}

	void CommandList::SetShaderValue( const agl::ShaderParameter& parameter, const void* value )
	{
		m_imple.SetShaderValue( parameter, value );
	}

	void CommandList::DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation )
	{
		m_imple.DrawInstanced( vertexCount, numInstance, baseVertexLocation );
	}

	void CommandList::DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
		m_imple.DrawIndexedInstanced( indexCount, numInstance, startIndexLocation, baseVertexLocation );
	}

	void CommandList::Dispatch( uint32 x, uint32 y, uint32 z )
	{
		m_imple.Dispatch( x, y, z );
		BindPipelineState( static_cast<agl::ComputePipelineState*>( nullptr ) );
	}

	void CommandList::SetViewports( uint32 count, const CubeArea<float>* areas )
	{
		m_imple.SetViewports( count, areas );
	}

	void CommandList::SetScissorRects( uint32 count, const RectangleArea<int32>* areas )
	{
		m_imple.SetScissorRects( count, areas );
	}

	void CommandList::BindRenderTargets( agl::RenderTargetView** pRenderTargets, uint32 renderTargetCount, agl::DepthStencilView* depthStencil )
	{
		m_imple.BindRenderTargets( pRenderTargets, renderTargetCount, depthStencil );
	}

	void CommandList::ClearRenderTarget( agl::RenderTargetView* renderTarget, const float( &clearColor )[4] )
	{
		m_imple.ClearRenderTarget( renderTarget, clearColor );
	}

	void CommandList::ClearDepthStencil( agl::DepthStencilView* depthStencil, float depthColor, UINT8 stencilColor )
	{
		m_imple.ClearDepthStencil( depthStencil, depthColor, stencilColor );
	}

	void CommandList::CopyResource( agl::Texture* dest, agl::Texture* src, bool bDirect )
	{
		m_imple.CopyResource( dest, src, bDirect );
	}

	void CommandList::CopyResource( agl::Buffer* dest, agl::Buffer* src, uint32 numByte, bool bDirect )
	{
		m_imple.CopyResource( dest, src, numByte, bDirect );
	}

	void CommandList::UpdateSubresource( agl::Texture* dest, const void* src, uint32 srcRowSize, const CubeArea<uint32>* destArea, uint32 subresource )
	{
		m_imple.UpdateSubresource( dest, src, srcRowSize, destArea, subresource );
	}

	void CommandList::UpdateSubresource( agl::Buffer* dest, const void* src, uint32 destOffset, uint32 numByte )
	{
		m_imple.UpdateSubresource( dest, src, destOffset, numByte );
	}

	void CommandList::AddTransition( const agl::ResourceTransition& transition )
	{
		m_imple.AddTransition( transition );
	}

	void CommandList::AddUavBarrier( const agl::UavBarrier& uavBarrier )
	{
		m_imple.AddUavBarrier( uavBarrier );
	}

	bool CommandList::CaptureTexture( agl::Texture* texture, DirectX::ScratchImage& outResult )
	{
		return m_imple.CaptureTexture( texture, outResult );
	}

	void CommandList::BeginQuery( agl::Query* rawQuery )
	{
		rawQuery->Begin( m_imple );
	}

	void CommandList::EndQuery( agl::Query* rawQuery )
	{
		rawQuery->End( m_imple );
	}

	void CommandList::WaitUntilFlush()
	{
		m_imple.WaitUntilFlush();
	}

	void CommandList::Commit()
	{
		m_imple.Commit();
	}

	CommandList GetCommandList()
	{
		auto commandList = GraphicsInterface().GetCommandList();
		return CommandList( *commandList );
	}
}
