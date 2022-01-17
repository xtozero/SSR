#pragma once

#include "D3D11StateCache.h"
#include "ICommandList.h"

struct ID3D11DeviceContext;
struct ID3D11CommandList;

namespace aga
{
	class D3D11ImmediateCommandList : public IImmediateCommandList
	{
	public:
		virtual void BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets ) override;
		virtual void BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset ) override;
		virtual void BindPipelineState( PipelineState* pipelineState ) override;
		virtual void BindShader( VertexShader* vs ) override;
		virtual void BindShader( GeometryShader* gs ) override;
		virtual void BindShader( PixelShader* ps ) override;
		virtual void BindShader( ComputeShader* cs ) override;
		virtual void BindShaderResources( const ShaderBindings& shaderBindings ) override;
		virtual void BindConstantBuffer( SHADER_TYPE shader, uint32 slot, Buffer* buffer ) override;
		virtual void BindSRV( SHADER_TYPE shader, uint32 slot, ShaderResourceView* srv ) override;
		virtual void BindUAV( SHADER_TYPE shader, uint32 slot, UnorderedAccessView* uav ) override;
		virtual void BindSampler( SHADER_TYPE shader, uint32 slot, SamplerState* sampler ) override;
		virtual void DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation ) override;
		virtual void DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation ) override;
		virtual void Dispatch( uint32 x, uint32 y, uint32 z = 1 ) override;

		virtual void SetViewports( uint32 count, const CubeArea<float>* area ) override;
		virtual void SetScissorRects( uint32 count, const RectangleArea<int32>* area ) override;

		virtual void BindRenderTargets( aga::Texture** pRenderTargets, uint32 renderTargetCount, aga::Texture* depthStencil ) override;

		virtual void ClearRenderTarget( Texture* renderTarget, const float( &clearColor )[4] ) override;
		virtual void ClearDepthStencil( Texture* depthStencil, float depthColor, UINT8 stencilColor ) override;

		virtual void CopyResource( Texture* dest, Texture* src ) override;
		virtual void CopyResource( Buffer* dest, Buffer* src ) override;

		virtual void WaitUntilFlush() override;

		virtual void Execute( IDeferredCommandList& commandList ) override;

	private:
		D3D11PipelineCache m_stateCache;
	};

	class D3D11DeferredCommandList : public IDeferredCommandList
	{
	public:
		virtual void BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets ) override;
		virtual void BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset ) override;
		virtual void BindPipelineState( PipelineState* pipelineState ) override;
		virtual void BindShader( VertexShader* vs ) override;
		virtual void BindShader( GeometryShader* gs ) override;
		virtual void BindShader( PixelShader* ps ) override;
		virtual void BindShader( ComputeShader* cs ) override;
		virtual void BindShaderResources( const ShaderBindings& shaderBindings ) override;
		virtual void BindConstantBuffer( SHADER_TYPE shader, uint32 slot, Buffer* buffer ) override;
		virtual void BindSRV( SHADER_TYPE shader, uint32 slot, ShaderResourceView* srv ) override;
		virtual void BindUAV( SHADER_TYPE shader, uint32 slot, UnorderedAccessView* uav ) override;
		virtual void BindSampler( SHADER_TYPE shader, uint32 slot, SamplerState* sampler ) override;
		virtual void DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation ) override;
		virtual void DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation ) override;
		virtual void Dispatch( uint32 x, uint32 y, uint32 z = 1 ) override;

		virtual void SetViewports( uint32 count, const CubeArea<float>* areas ) override;
		virtual void SetScissorRects( uint32 count, const RectangleArea<int32>* areas ) override;

		virtual void BindRenderTargets( aga::Texture** pRenderTargets, uint32 renderTargetCount, aga::Texture* depthStencil ) override;

		virtual void ClearRenderTarget( Texture* renderTarget, const float( &clearColor )[4] ) override;
		virtual void ClearDepthStencil( Texture* depthStencil, float depthColor, UINT8 stencilColor ) override;

		virtual void CopyResource( Texture* dest, Texture* src ) override;
		virtual void CopyResource( Buffer* dest, Buffer* src ) override;

		virtual void Finish() override;

		void RequestExecute();

		D3D11DeferredCommandList();
		virtual ~D3D11DeferredCommandList();

	private:
		ID3D11DeviceContext* m_pContext = nullptr;
		ID3D11CommandList* m_pCommandList = nullptr;
		D3D11PipelineCache m_stateCache;
	};
}
