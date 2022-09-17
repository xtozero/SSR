#include "CommandList.h"

#include "AbstractGraphicsInterface.h"
#include "DefaultConstantBuffers.h"
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

	void ImmediateCommandList::BindPipelineState( agl::PipelineState* pipelineState )
	{
		m_imple.BindPipelineState( pipelineState );
	}

	void ImmediateCommandList::BindShader( agl::VertexShader* vs )
	{
		m_imple.BindShader( vs );
	}

	void ImmediateCommandList::BindShader( agl::GeometryShader* gs )
	{
		m_imple.BindShader( gs );
	}

	void ImmediateCommandList::BindShader( agl::PixelShader* ps )
	{
		m_imple.BindShader( ps );
	}

	void ImmediateCommandList::BindShader( agl::ComputeShader* cs )
	{
		m_imple.BindShader( cs );
	}

	void ImmediateCommandList::BindShaderResources( const agl::ShaderBindings& shaderBindings )
	{
		m_imple.BindShaderResources( shaderBindings );
	}

	void ImmediateCommandList::BindConstantBuffer( agl::ShaderType shader, uint32 slot, agl::Buffer* buffer )
	{
		m_imple.BindConstantBuffer( shader, slot, buffer );
	}

	void ImmediateCommandList::BindSRV( agl::ShaderType shader, uint32 slot, agl::ShaderResourceView* srv )
	{
		m_imple.BindSRV( shader, slot, srv );
	}

	void ImmediateCommandList::BindUAV( agl::ShaderType shader, uint32 slot, agl::UnorderedAccessView* uav )
	{
		m_imple.BindUAV( shader, slot, uav );
	}

	void ImmediateCommandList::BindSampler( agl::ShaderType shader, uint32 slot, agl::SamplerState* sampler )
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
		DefaultConstantBuffers::GetInstance().Commit( agl::ShaderType::CS );
		m_imple.Dispatch( x, y, z );
		BindShader( static_cast<agl::ComputeShader*>( nullptr ) );
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
