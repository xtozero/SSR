#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
#include "ICommandList.h"
#include "Util.h"

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
		virtual bool BootUp( HWND hWnd, UINT nWndWidth, UINT nWndHeight ) = 0;
		virtual void HandleDeviceLost( HWND hWnd, UINT nWndWidth, UINT nWndHeight ) = 0;
		virtual void AppSizeChanged( UINT nWndWidth, UINT nWndHeight ) = 0;
		virtual void WaitGPU( ) = 0;

		virtual void* Lock( Buffer* buffer, int lockFlag = BUFFER_LOCKFLAG::WRITE_DISCARD, UINT subResource = 0 ) = 0;
		virtual void UnLock( Buffer* buffer, UINT subResource = 0 ) = 0;

		virtual void SetViewports( Viewport** viewPorts, int count ) = 0;
		virtual void SetViewport( UINT minX, UINT minY, float minZ, UINT maxX, UINT maxY, float maxZ ) = 0;
		virtual void SetScissorRects( Viewport** viewPorts, int size ) = 0;
		virtual void SetScissorRect( UINT minX, UINT minY, UINT maxX, UINT maxY ) = 0;

		virtual void ClearDepthStencil( Texture* depthStencil, float depthColor, UINT8 stencilColor ) = 0;

		virtual void BindShader( ComputeShader* shader ) = 0;

		virtual void BindConstant( VertexShader* shader, int startSlot, int numBuffers, Buffer** pBuffers ) = 0;
		virtual void BindShaderInput( VertexShader* shader, int startSlot, int numBuffers, Buffer** pBuffers ) = 0;

		virtual void BindConstant( PixelShader* shader, int startSlot, int numBuffers, Buffer** pBuffers ) = 0;
		virtual void BindShaderInput( PixelShader* shader, int startSlot, int numBuffers, Buffer** pBuffers ) = 0;

		virtual void BindConstant( ComputeShader* shader, int startSlot, int numBuffers, Buffer** pBuffers ) = 0;
		virtual void BindShaderInput( ComputeShader* shader, int startSlot, int numBuffers, Buffer** pBuffers ) = 0;
		virtual void BindShaderOutput( ComputeShader* shader, int startSlot, int numBuffers, Buffer** pBuffers ) = 0;

		virtual void BindRenderTargets( Texture** pRenderTargets, int renderTargetCount, Texture* depthStencil ) = 0;

		virtual void Dispatch( UINT x, UINT y, UINT z = 1 ) = 0;

		virtual void Copy( Buffer* dst, Buffer* src, std::size_t size ) = 0;

		virtual void GetRendererMultiSampleOption( MULTISAMPLE_OPTION* option ) = 0;

		virtual std::unique_ptr<IImmediateCommandList> GetImmediateCommandList( ) const = 0;

		virtual ~IAga( ) = default;

	protected:
		IAga( ) = default;
	};
}
