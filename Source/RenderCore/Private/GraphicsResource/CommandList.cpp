#include "CommandList.h"

#include "AbstractGraphicsInterface.h"
#include "DefaultConstantBuffers.h"
#include "InterfaceFactories.h"

namespace rendercore
{
	void ImmediateCommandList::BindVertexBuffer( aga::Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets )
	{
		m_imple.BindVertexBuffer( vertexBuffers, startSlot, numBuffers, pOffsets );
	}

	void ImmediateCommandList::BindIndexBuffer( aga::Buffer* indexBuffer, uint32 indexOffset )
	{
		m_imple.BindIndexBuffer( indexBuffer, indexOffset );
	}

	void ImmediateCommandList::BindPipelineState( aga::PipelineState* pipelineState )
	{
		m_imple.BindPipelineState( pipelineState );
	}

	void ImmediateCommandList::BindShader( aga::VertexShader* vs )
	{
		m_imple.BindShader( vs );
	}

	void ImmediateCommandList::BindShader( aga::GeometryShader* gs )
	{
		m_imple.BindShader( gs );
	}

	void ImmediateCommandList::BindShader( aga::PixelShader* ps )
	{
		m_imple.BindShader( ps );
	}

	void ImmediateCommandList::BindShader( aga::ComputeShader* cs )
	{
		m_imple.BindShader( cs );
	}

	void ImmediateCommandList::BindShaderResources( const aga::ShaderBindings& shaderBindings )
	{
		m_imple.BindShaderResources( shaderBindings );
	}

	void ImmediateCommandList::BindConstantBuffer( SHADER_TYPE shader, uint32 slot, aga::Buffer* buffer )
	{
		m_imple.BindConstantBuffer( shader, slot, buffer );
	}

	void ImmediateCommandList::BindSRV( SHADER_TYPE shader, uint32 slot, aga::ShaderResourceView* srv )
	{
		m_imple.BindSRV( shader, slot, srv );
	}

	void ImmediateCommandList::BindUAV( SHADER_TYPE shader, uint32 slot, aga::UnorderedAccessView* uav )
	{
		m_imple.BindUAV( shader, slot, uav );
	}

	void ImmediateCommandList::BindSampler( SHADER_TYPE shader, uint32 slot, aga::SamplerState* sampler )
	{
		m_imple.BindSampler( shader, slot, sampler );
	}

	void ImmediateCommandList::DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation )
	{
		DefaultConstantBuffers::GetInstance().CommitAll();
		m_imple.DrawInstanced( vertexCount, numInstance, baseVertexLocation );
	}

	void ImmediateCommandList::DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation )
	{
		DefaultConstantBuffers::GetInstance().CommitAll();
		m_imple.DrawIndexedInstanced( indexCount, numInstance, startIndexLocation, baseVertexLocation );
	}

	void ImmediateCommandList::Dispatch( uint32 x, uint32 y, uint32 z )
	{
		DefaultConstantBuffers::GetInstance().Commit( SHADER_TYPE::CS );
		m_imple.Dispatch( x, y, z );
		BindShader( static_cast<aga::ComputeShader*>( nullptr ) );
	}

	void ImmediateCommandList::SetViewports( uint32 count, const CubeArea<float>* areas )
	{
		m_imple.SetViewports( count, areas );
	}

	void ImmediateCommandList::SetScissorRects( uint32 count, const RectangleArea<int32>* areas )
	{
		m_imple.SetScissorRects( count, areas );
	}

	void ImmediateCommandList::BindRenderTargets( aga::RenderTargetView** pRenderTargets, uint32 renderTargetCount, aga::DepthStencilView* depthStencil )
	{
		m_imple.BindRenderTargets( pRenderTargets, renderTargetCount, depthStencil );
	}

	void ImmediateCommandList::ClearRenderTarget( aga::RenderTargetView* renderTarget, const float( &clearColor )[4] )
	{
		m_imple.ClearRenderTarget( renderTarget, clearColor );
	}

	void ImmediateCommandList::ClearDepthStencil( aga::DepthStencilView* depthStencil, float depthColor, UINT8 stencilColor )
	{
		m_imple.ClearDepthStencil( depthStencil, depthColor, stencilColor );
	}

	void ImmediateCommandList::CopyResource( aga::Texture* dest, aga::Texture* src )
	{
		m_imple.CopyResource( dest, src );
	}

	void ImmediateCommandList::CopyResource( aga::Buffer* dest, aga::Buffer* src )
	{
		m_imple.CopyResource( dest, src );
	}

	void ImmediateCommandList::WaitUntilFlush()
	{
		m_imple.WaitUntilFlush();
	}

	void ImmediateCommandList::Execute( aga::IDeferredCommandList& commandList )
	{
		m_imple.Execute( commandList );
	}

	ImmediateCommandList GetImmediateCommandList()
	{
		auto commandList = GraphicsInterface().GetImmediateCommandList();
		return ImmediateCommandList( *commandList );
	}
}
