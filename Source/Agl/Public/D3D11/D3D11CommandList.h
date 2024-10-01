#pragma once

#include "D3D11StateCache.h"
#include "GlobalConstantBuffers.h"
#include "ICommandList.h"
#include "Memory/InlineMemoryAllocator.h"

#include <d3d11_1.h>
#include <wrl/client.h>

struct ID3D11DeviceContext;
struct ID3D11CommandList;

namespace agl
{
	class D3D11CommandList final : public ICommandList
	{
	public:
		virtual void Prepare() override;

		virtual void BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* strides, const uint32* pOffsets ) override;
		virtual void BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset ) override;
		virtual void BindPipelineState( GraphicsPipelineState* pipelineState ) override;
		virtual void BindPipelineState( ComputePipelineState* pipelineState ) override;
		virtual void BindShaderResources( ShaderBindings& shaderBindings ) override;
		virtual void SetShaderValue( const ShaderParameter& parameter, const void* value ) override;
		virtual void DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation ) override;
		virtual void DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation ) override;
		virtual void Dispatch( uint32 x, uint32 y, uint32 z = 1 ) override;

		virtual void SetViewports( uint32 count, const CubeArea<float>* area ) override;
		virtual void SetScissorRects( uint32 count, const RectangleArea<int32>* area ) override;

		virtual void BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil ) override;

		virtual void ClearRenderTarget( RenderTargetView* renderTarget, const float( &clearColor )[4] ) override;
		virtual void ClearDepthStencil( DepthStencilView* depthStencil, float depthColor, UINT8 stencilColor ) override;

		virtual void CopyResource( Texture* dest, Texture* src, bool bDirect = false ) override;
		virtual void CopyResource( Buffer* dest, Buffer* src, uint32 numByte = 0, bool bDirect = false ) override;

		virtual void UpdateSubresource( agl::Texture* dest, const void* src, uint32 srcRowSize, const CubeArea<uint32>* destArea = nullptr, uint32 subresource = 0 ) override;
		virtual void UpdateSubresource( agl::Buffer* dest, const void* src, uint32 destOffset = 0, uint32 numByte = 0 ) override;

		virtual void AddTransition( const ResourceTransition& transition ) override;
		virtual void AddUavBarrier( const UavBarrier& uavBarrier ) override;

		virtual bool CaptureTexture( agl::Texture* texture, DirectX::ScratchImage& outResult ) override;

		virtual void BeginQuery( void* rawQuery ) override;
		virtual void EndQuery( void* rawQuery ) override;

		virtual void BeginEvent( const char* eventName ) override;
		virtual void EndEvent() override;

		virtual void Commit() override;

		void Initialize();

		ICommandList& GetParallelCommandList();

		D3D11CommandList() = default;
		D3D11CommandList( const D3D11CommandList& ) = delete;
		D3D11CommandList& operator=( const D3D11CommandList& ) = delete;
		D3D11CommandList( D3D11CommandList&& other ) noexcept;
		D3D11CommandList& operator=( D3D11CommandList&& other ) noexcept;
		virtual ~D3D11CommandList();

	private:
		Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation> m_annotation;

		D3D11PipelineCache m_stateCache;
		GlobalSyncConstantBuffers m_globalConstantBuffers;

		uint32 m_numUsedParallelCommandList = 0;
		std::vector<ICommandList*, InlineAllocator<ICommandList*, 1>> m_parallelCommandLists;
	};

	class D3D11ParallelCommandList final : public ICommandList
	{
	public:
		virtual void Prepare() override;

		virtual void BindVertexBuffer( Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* strides, const uint32* pOffsets ) override;
		virtual void BindIndexBuffer( Buffer* indexBuffer, uint32 indexOffset ) override;
		virtual void BindPipelineState( GraphicsPipelineState* pipelineState ) override;
		virtual void BindPipelineState( ComputePipelineState* pipelineState ) override;
		virtual void BindShaderResources( ShaderBindings& shaderBindings ) override;
		virtual void SetShaderValue( const ShaderParameter& parameter, const void* value ) override;
		virtual void DrawInstanced( uint32 vertexCount, uint32 numInstance, uint32 baseVertexLocation ) override;
		virtual void DrawIndexedInstanced( uint32 indexCount, uint32 numInstance, uint32 startIndexLocation, uint32 baseVertexLocation ) override;
		virtual void Dispatch( uint32 x, uint32 y, uint32 z = 1 ) override;

		virtual void SetViewports( uint32 count, const CubeArea<float>* areas ) override;
		virtual void SetScissorRects( uint32 count, const RectangleArea<int32>* areas ) override;

		virtual void BindRenderTargets( RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil ) override;

		virtual void ClearRenderTarget( RenderTargetView* renderTarget, const float( &clearColor )[4] ) override;
		virtual void ClearDepthStencil( DepthStencilView* depthStencil, float depthColor, UINT8 stencilColor ) override;

		virtual void CopyResource( Texture* dest, Texture* src, bool bDirect = false ) override;
		virtual void CopyResource( Buffer* dest, Buffer* src, uint32 numByte = 0, bool bDirect = false ) override;

		virtual void UpdateSubresource( agl::Texture* dest, const void* src, uint32 srcRowSize, const CubeArea<uint32>* destArea = nullptr, uint32 subresource = 0 ) override;
		virtual void UpdateSubresource( agl::Buffer* dest, const void* src, uint32 srcSize, uint32 subresource = 0 ) override;

		virtual void AddTransition( const ResourceTransition& transition ) override;
		virtual void AddUavBarrier( const UavBarrier& uavBarrier ) override;

		virtual bool CaptureTexture( agl::Texture* texture, DirectX::ScratchImage& outResult ) override;

		virtual void BeginQuery( void* rawQuery ) override;
		virtual void EndQuery( void* rawQuery ) override;

		virtual void BeginEvent( const char* eventName ) override;
		virtual void EndEvent() override;

		virtual void Commit() override;

		void Close();

		void Initialize();

	private:
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pContext;
		Microsoft::WRL::ComPtr<ID3D11CommandList> m_pCommandList;
		Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation> m_annotation;

		D3D11PipelineCache m_stateCache;
		GlobalAsyncConstantBuffers m_globalConstantBuffers;
	};
}
