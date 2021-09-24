#pragma once

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
		virtual void BindShaderResources( const ShaderBindings& shaderBindings ) override;
		virtual void Draw( uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation ) override;
		virtual void DrawInstancing( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation ) override;

		virtual void SetViewports( uint32 count, const CubeArea<float>* area ) override;
		virtual void SetScissorRects( uint32 count, const RectangleArea<int32>* area ) override;

		virtual void BindRenderTargets( aga::Texture** pRenderTargets, uint32 renderTargetCount, aga::Texture* depthStencil ) override;

		virtual void Execute( IDeferredCommandList& commandList ) override;
	};

	class D3D11DeferredCommandList : public IDeferredCommandList
	{
	public:
		virtual void BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets ) override;
		virtual void BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset ) override;
		virtual void BindPipelineState( PipelineState* pipelineState ) override;
		virtual void BindShaderResources( const ShaderBindings& shaderBindings ) override;
		virtual void Draw( uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation ) override;
		virtual void DrawInstancing( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation ) override;

		virtual void SetViewports( uint32 count, const CubeArea<float>* areas ) override;
		virtual void SetScissorRects( uint32 count, const RectangleArea<int32>* areas ) override;

		virtual void BindRenderTargets( aga::Texture** pRenderTargets, uint32 renderTargetCount, aga::Texture* depthStencil ) override;

		virtual void Finish( ) override;

		void RequestExecute( );

		D3D11DeferredCommandList( );
		virtual ~D3D11DeferredCommandList( );

	private:
		ID3D11DeviceContext* m_pContext = nullptr;
		ID3D11CommandList* m_pCommandList = nullptr;
	};
}
