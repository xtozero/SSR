#include "CommandList.h"

#include "AbstractGraphicsInterface.h"
#include "InterfaceFactories.h"
#include "Query.h"

namespace rendercore
{
	void ComputeCommandList::BindPipelineState( agl::ComputePipelineState* pipelineState )
	{
		m_imple.BindPipelineState( pipelineState );
	}

	void ComputeCommandList::BindShaderResources( agl::ShaderBindings& shaderBindings )
	{
		m_imple.BindShaderResources( shaderBindings );
	}

	void ComputeCommandList::SetShaderValue( const agl::ShaderParameter& parameter, const void* value )
	{
		m_imple.SetShaderValue( parameter, value );
	}

	void ComputeCommandList::Dispatch( uint32 x, uint32 y, uint32 z )
	{
		m_imple.Dispatch( x, y, z );
		BindPipelineState( static_cast<agl::ComputePipelineState*>( nullptr ) );
	}

	void ComputeCommandList::AddTransition( const agl::ResourceTransition& transition )
	{
		m_imple.AddTransition( transition );
	}

	void ComputeCommandList::AddUavBarrier( const agl::UavBarrier& uavBarrier )
	{
		m_imple.AddUavBarrier( uavBarrier );
	}

	void ComputeCommandList::BeginQuery( agl::Query* rawQuery )
	{
		rawQuery->Begin( m_imple );
	}

	void ComputeCommandList::EndQuery( agl::Query* rawQuery )
	{
		rawQuery->End( m_imple );
	}

	void ComputeCommandList::BeginEvent( const char* eventName )
	{
		m_imple.BeginEvent( eventName );
	}

	void ComputeCommandList::EndEvent()
	{
		m_imple.EndEvent();
	}

	void ComputeCommandList::Commit()
	{
		m_imple.Commit();
	}

	void ResourceCommandList::CopyResource( agl::Texture* dest, agl::Texture* src, bool bAsync )
	{
		m_imple.CopyResource( dest, src, bAsync );
	}

	void ResourceCommandList::CopyResource( agl::Buffer* dest, agl::Buffer* src, bool bAsync, uint32 numByte )
	{
		m_imple.CopyResource( dest, src, bAsync, numByte );
	}

	void ResourceCommandList::UpdateSubresource( agl::Texture* dest, const void* src, uint32 srcRowSize, bool bAsync, const CubeArea<uint32>* destArea, uint32 subresource )
	{
		m_imple.UpdateSubresource( dest, src, srcRowSize, bAsync, destArea, subresource );
	}

	void ResourceCommandList::UpdateSubresource( agl::Buffer* dest, const void* src, bool bAsync, uint32 destOffset, uint32 numByte )
	{
		m_imple.UpdateSubresource( dest, src, bAsync, destOffset, numByte );
	}

	bool ResourceCommandList::CaptureTexture( agl::Texture* texture, DirectX::ScratchImage& outResult )
	{
		return m_imple.CaptureTexture( texture, outResult );
	}

	void ResourceCommandList::ClearRenderTarget( agl::RenderTargetView* renderTarget )
	{
		m_imple.ClearRenderTarget( renderTarget );
	}

	void ResourceCommandList::ClearDepthStencil( agl::DepthStencilView* depthStencil )
	{
		m_imple.ClearDepthStencil( depthStencil );
	}

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

	void CommandList::DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation )
	{
		m_imple.DrawInstanced( vertexCount, numInstance, baseVertexLocation );
	}

	void CommandList::DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
		m_imple.DrawIndexedInstanced( indexCount, numInstance, startIndexLocation, baseVertexLocation );
	}

	void CommandList::DispatchMesh( uint32 x, uint32 y, uint32 z )
	{
		assert( GetInterface<agl::IAgl>()->IsSupportsMeshShader() );
		m_imple.DispatchMesh( x, y, z );
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

	CommandList GetCommandList()
	{
		auto commandList = GraphicsInterface().GetCommandList();
		return CommandList( *commandList );
	}
}
