#include "CommandList.h"

#include "AbstractGraphicsInterface.h"
#include "InterfaceFactories.h"
#include "Query.h"

namespace rendercore
{
	void CopyCommandList::CopyResource( agl::Texture* dest, agl::Texture* src, bool bAsync ) const
	{
		GetImpl().CopyResource( dest, src, bAsync );
	}

	void CopyCommandList::CopyResource( agl::Buffer* dest, agl::Buffer* src, bool bAsync, uint32 numByte ) const
	{
		GetImpl().CopyResource( dest, src, bAsync, numByte );
	}

	void CopyCommandList::UpdateSubresource( agl::Texture* dest, const void* src, uint32 srcRowSize, bool bAsync, const CubeArea<uint32>* destArea, uint32 subresource ) const
	{
		GetImpl().UpdateSubresource( dest, src, srcRowSize, bAsync, destArea, subresource );
	}

	void CopyCommandList::UpdateSubresource( agl::Buffer* dest, const void* src, bool bAsync, uint32 destOffset, uint32 numByte ) const
	{
		GetImpl().UpdateSubresource( dest, src, bAsync, destOffset, numByte );
	}

	void CopyCommandList::Commit() const
	{
		GetImpl().Commit();
	}

	agl::ICopyCommandList& CopyCommandList::GetImpl() const
	{
		return static_cast<agl::ICopyCommandList&>( m_impl );
	}

	void ComputeCommandList::BindPipelineState( agl::ComputePipelineState* pipelineState ) const
	{
		GetImpl().BindPipelineState( pipelineState );
	}

	void ComputeCommandList::BindShaderResources( agl::ShaderBindings& shaderBindings ) const
	{
		GetImpl().BindShaderResources( shaderBindings );
	}

	void ComputeCommandList::SetShaderValue( const agl::ShaderParameter& parameter, const void* value ) const
	{
		GetImpl().SetShaderValue( parameter, value );
	}

	void ComputeCommandList::Dispatch( uint32 x, uint32 y, uint32 z ) const
	{
		GetImpl().Dispatch( x, y, z );
		BindPipelineState( nullptr );
	}

	void ComputeCommandList::ExecuteIndirect( agl::IndirectCommandType type, agl::Buffer* argument,
		uint64 argumentOffset )
	{
		GetImpl().ExecuteIndirect( type, argument, argumentOffset );
		if ( type == agl::IndirectCommandType::Dispatch )
		{
			BindPipelineState( nullptr );
		}
	}

	void ComputeCommandList::AddTransition( const agl::ResourceTransition& transition ) const
	{
		m_impl.AddTransition( transition );
	}

	void ComputeCommandList::AddUavBarrier( const agl::UavBarrier& uavBarrier ) const
	{
		m_impl.AddUavBarrier( uavBarrier );
	}

	void ComputeCommandList::BeginQuery( agl::Query* rawQuery ) const
	{
		rawQuery->Begin( m_impl );
	}

	void ComputeCommandList::EndQuery( agl::Query* rawQuery ) const
	{
		rawQuery->End( m_impl );
	}

	void ComputeCommandList::BeginEvent( const char* eventName ) const
	{
		m_impl.BeginEvent( eventName );
	}

	void ComputeCommandList::EndEvent() const
	{
		m_impl.EndEvent();
	}

	agl::IComputeCommandList& ComputeCommandList::GetImpl() const
	{
		return static_cast<agl::IComputeCommandList&>( m_impl );
	}

	void CommandList::BindVertexBuffer( agl::Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* strides, const uint32* pOffsets ) const
	{
		GetImpl().BindVertexBuffer( vertexBuffers, startSlot, numBuffers, strides, pOffsets );
	}

	void CommandList::BindIndexBuffer( agl::Buffer* indexBuffer, uint32 indexOffset ) const
	{
		GetImpl().BindIndexBuffer( indexBuffer, indexOffset );
	}

	void CommandList::BindPipelineState( agl::GraphicsPipelineState* pipelineState ) const
	{
		GetImpl().BindPipelineState( pipelineState );
	}

	void CommandList::BindRenderTargets( agl::RenderTargetView** pRenderTargets, uint32 renderTargetCount, agl::DepthStencilView* depthStencil ) const
	{
		GetImpl().BindRenderTargets( pRenderTargets, renderTargetCount, depthStencil );
	}

	void CommandList::DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation ) const
	{
		GetImpl().DrawInstanced( vertexCount, numInstance, baseVertexLocation );
	}

	void CommandList::DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation ) const
	{
		GetImpl().DrawIndexedInstanced( indexCount, numInstance, startIndexLocation, baseVertexLocation );
	}

	void CommandList::DispatchMesh( uint32 x, uint32 y, uint32 z ) const
	{
		assert( GetInterface<agl::IAgl>()->SupportsMeshShader() );
		GetImpl().DispatchMesh( x, y, z );
	}

	void CommandList::SetViewports( uint32 count, const CubeArea<float>* areas ) const
	{
		GetImpl().SetViewports( count, areas );
	}

	void CommandList::SetScissorRects( uint32 count, const RectangleArea<int32>* areas ) const
	{
		GetImpl().SetScissorRects( count, areas );
	}

	void CommandList::ClearRenderTarget( agl::RenderTargetView* renderTarget ) const
	{
		GetImpl().ClearRenderTarget( renderTarget );
	}

	void CommandList::ClearDepthStencil( agl::DepthStencilView* depthStencil ) const
	{
		GetImpl().ClearDepthStencil( depthStencil );
	}

	bool CommandList::CaptureTexture( agl::Texture* texture, DirectX::ScratchImage& outResult ) const
	{
		return GetImpl().CaptureTexture( texture, outResult );
	}

	agl::ICommandList& CommandList::GetImpl() const
	{
		return static_cast<agl::ICommandList&>( m_impl );
	}

	CommandList GetCommandList()
	{
		auto commandList = GraphicsInterface().GetCommandList();
		return CommandList( *commandList );
	}

	ComputeCommandList GetComputeCommandList()
	{
		auto commandList = GraphicsInterface().GetComputeCommandList();
		return ComputeCommandList( *commandList );
	}
}
