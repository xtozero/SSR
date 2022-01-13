#pragma once

#include "ICommandList.h"

#include <memory>

namespace rendercore
{
	class ImmediateCommandList
	{
	public:
		void BindVertexBuffer( aga::Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets );
		void BindIndexBuffer( aga::Buffer* indexBuffer, uint32 indexOffset );

		void BindPipelineState( aga::PipelineState* pipelineState );
		void BindShader( aga::VertexShader* vs );
		void BindShader( aga::GeometryShader* gs );
		void BindShader( aga::PixelShader* ps );
		void BindShader( aga::ComputeShader* cs );
		void BindShaderResources( const aga::ShaderBindings& shaderBindings );
		void BindConstantBuffer( SHADER_TYPE shader, uint32 slot, aga::Buffer* buffer );
		void BindSRV( SHADER_TYPE shader, uint32 slot, aga::ShaderResourceView* srv );
		void BindUAV( SHADER_TYPE shader, uint32 slot, aga::UnorderedAccessView* uav );
		void BindSampler( SHADER_TYPE shader, uint32 slot, aga::SamplerState* sampler );
		void DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation );
		void DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation );
		void Dispatch( uint32 x, uint32 y, uint32 z = 1 );

		void SetViewports( uint32 count, const CubeArea<float>* areas );
		void SetScissorRects( uint32 count, const RectangleArea<int32>* areas );

		void BindRenderTargets( aga::Texture** pRenderTargets, uint32 renderTargetCount, aga::Texture* depthStencil );

		void ClearRenderTarget( aga::Texture* renderTarget, const float( &clearColor )[4] );
		void ClearDepthStencil( aga::Texture* depthStencil, float depthColor, UINT8 stencilColor );

		void Execute( aga::IDeferredCommandList& commandList );

		explicit ImmediateCommandList( aga::IImmediateCommandList& imple )
			: m_imple( imple ) {}

	private:
		aga::IImmediateCommandList& m_imple;
	};

	ImmediateCommandList GetImmediateCommandList();
}
