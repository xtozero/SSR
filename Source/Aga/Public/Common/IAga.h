#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
#include "ICommandList.h"
#include "SizedTypes.h"

#include <memory>

namespace aga
{
	class BlendState;
	class Buffer;
	class ComputeShader;
	class DepthStencilState;
	class PixelShader;
	class RasterizerState;
	class PipelineState;
	class PipelineStateInitializer;
	class SamplerState;
	class Texture;
	class VertexLayout;
	class VertexShader;
	class Viewport;

	class IAga
	{
	public:
		virtual bool BootUp( ) = 0;
		virtual void HandleDeviceLost( ) = 0;
		virtual void AppSizeChanged( ) = 0;
		virtual void WaitGPU( ) = 0;

		virtual void* Lock( Buffer* buffer, uint32 lockFlag = BUFFER_LOCKFLAG::WRITE_DISCARD, uint32 subResource = 0 ) = 0;
		virtual void UnLock( Buffer* buffer, uint32 subResource = 0 ) = 0;

		virtual void SetViewports( Viewport** viewPorts, uint32 count ) = 0;
		virtual void SetViewport( uint32 minX, uint32 minY, float minZ, uint32 maxX, uint32 maxY, float maxZ ) = 0;
		virtual void SetScissorRects( Viewport** viewPorts, uint32 size ) = 0;
		virtual void SetScissorRect( uint32 minX, uint32 minY, uint32 maxX, uint32 maxY ) = 0;

		virtual void ClearDepthStencil( Texture* depthStencil, float depthColor, uint8 stencilColor ) = 0;

		virtual void BindShader( ComputeShader* shader ) = 0;

		virtual void BindConstant( VertexShader* shader, uint32 startSlot, uint32 numBuffers, Buffer** pBuffers ) = 0;
		virtual void BindShaderInput( VertexShader* shader, uint32 startSlot, uint32 numBuffers, Buffer** pBuffers ) = 0;

		virtual void BindConstant( PixelShader* shader, uint32 startSlot, uint32 numBuffers, Buffer** pBuffers ) = 0;
		virtual void BindShaderInput( PixelShader* shader, uint32 startSlot, uint32 numBuffers, Buffer** pBuffers ) = 0;

		virtual void BindConstant( ComputeShader* shader, uint32 startSlot, uint32 numBuffers, Buffer** pBuffers ) = 0;
		virtual void BindShaderInput( ComputeShader* shader, uint32 startSlot, uint32 numBuffers, Buffer** pBuffers ) = 0;
		virtual void BindShaderOutput( ComputeShader* shader, uint32 startSlot, uint32 numBuffers, Buffer** pBuffers ) = 0;

		virtual void BindRenderTargets( Texture** pRenderTargets, uint32 renderTargetCount, Texture* depthStencil ) = 0;

		virtual void Dispatch( uint32 x, uint32 y, uint32 z = 1 ) = 0;

		virtual void Copy( Buffer* dst, Buffer* src, uint32 size ) = 0;

		virtual void GetRendererMultiSampleOption( MULTISAMPLE_OPTION* option ) = 0;

		virtual std::unique_ptr<IImmediateCommandList> GetImmediateCommandList( ) const = 0;
		virtual std::unique_ptr<IDeferredCommandList> CreateDeferredCommandList( ) const = 0;

		virtual ~IAga( ) = default;

	protected:
		IAga( ) = default;
	};
}
