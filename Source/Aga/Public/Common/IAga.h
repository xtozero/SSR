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
}

class IResourceManager;
class ShaderParameterInfo;
struct CXMFLOAT4X4;
struct IDXGISwapChain;

class IAga
{
public:
	virtual bool BootUp( HWND hWnd, UINT nWndWidth, UINT nWndHeight ) = 0;
	virtual void HandleDeviceLost( HWND hWnd, UINT nWndWidth, UINT nWndHeight ) = 0;
	virtual void AppSizeChanged( UINT nWndWidth, UINT nWndHeight ) = 0;
	virtual void WaitGPU( ) = 0;

	virtual aga::Texture* CreateTexture( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr ) = 0;

	virtual aga::Buffer* CreateBuffer( const BUFFER_TRAIT& trait, const void* initData = nullptr ) = 0;

	virtual aga::VertexLayout* CreateVertexLayout( const aga::VertexShader* vs, const VERTEX_LAYOUT_TRAIT* trait, std::size_t size ) = 0;
	// virtual RE_HANDLE CreateGeometryShader( const void* byteCode, std::size_t byteCodeSize ) = 0;
	virtual aga::ComputeShader* CreateComputeShader( const void* byteCode, std::size_t byteCodeSize ) = 0;
	virtual aga::VertexShader* CreateVertexShader( const void* byteCode, std::size_t byteCodeSize ) = 0;
	virtual aga::PixelShader* CreatePixelShader( const void* byteCode, std::size_t byteCodeSize ) = 0;
	// virtual const ShaderParameterInfo& GetShaderParameterInfo( RE_HANDLE shader ) const = 0;

	virtual aga::BlendState* CreateBlendState( const BLEND_STATE_TRAIT& trait ) = 0;
	virtual aga::DepthStencilState* CreateDepthStencilState( const DEPTH_STENCIL_STATE_TRAIT& trait ) = 0;
	virtual aga::RasterizerState* CreateRasterizerState( const RASTERIZER_STATE_TRAIT& trait ) = 0;
	virtual aga::SamplerState* CreateSamplerState( const SAMPLER_STATE_TRAIT& trait ) = 0;
	virtual aga::PipelineState* CreatePipelineState( const aga::PipelineStateInitializer& initializer ) = 0;

	virtual aga::Viewport* CreateViewport( int width, int height, void* hWnd, RESOURCE_FORMAT format ) = 0;

	virtual void* Lock( aga::Buffer* buffer, int lockFlag = BUFFER_LOCKFLAG::WRITE_DISCARD, UINT subResource = 0 ) = 0;
	virtual void UnLock( aga::Buffer* buffer, UINT subResource = 0 ) = 0;

	virtual void SetViewports( const aga::Viewport* viewPorts, int count ) = 0;
	virtual void SetViewport( UINT minX, UINT minY, float minZ, UINT maxX, UINT maxY, float maxZ ) = 0;
	virtual void SetScissorRects( const aga::Viewport* viewPorts, int size ) = 0;
	virtual void SetScissorRect( UINT minX, UINT minY, UINT maxX, UINT maxY ) = 0;

	virtual void ClearDepthStencil( aga::Texture* depthStencil, float depthColor, UINT8 stencilColor ) = 0;

	virtual void BindVertexBuffer( aga::Buffer** vertexBuffers, UINT startSlot, UINT numBuffers, const UINT* pStrides, const UINT* pOffsets ) = 0;
	virtual void BindIndexBuffer( aga::Buffer* indexBuffer, UINT stride, UINT indexOffset ) = 0;
	virtual void BindConstantBuffer( SHADER_TYPE type, UINT startSlot, UINT numBuffers, aga::Buffer** pConstantBuffers ) = 0;
	// virtual void BindVertexLayout( RE_HANDLE layout ) = 0;
	// virtual void BindShader( RE_HANDLE shader ) = 0;
	virtual void BindShader( aga::ComputeShader* shader ) = 0;

	virtual void BindConstant( aga::VertexShader* shader, int startSlot, int numBuffers, aga::Buffer** pBuffers ) = 0;
	virtual void BindShaderInput( aga::VertexShader* shader, int startSlot, int numBuffers, aga::Buffer** pBuffers ) = 0;

	virtual void BindConstant( aga::PixelShader* shader, int startSlot, int numBuffers, aga::Buffer** pBuffers ) = 0;
	virtual void BindShaderInput( aga::PixelShader* shader, int startSlot, int numBuffers, aga::Buffer** pBuffers ) = 0;

	virtual void BindConstant( aga::ComputeShader* shader, int startSlot, int numBuffers, aga::Buffer** pBuffers ) = 0;
	virtual void BindShaderInput( aga::ComputeShader* shader, int startSlot, int numBuffers, aga::Buffer** pBuffers ) = 0;
	virtual void BindShaderOutput( aga::ComputeShader* shader, int startSlot, int numBuffers, aga::Buffer** pBuffers ) = 0;

	// virtual void BindRandomAccessResource( int startSlot, int count, RE_HANDLE* resource ) = 0;
	virtual void BindRenderTargets( aga::Texture** pRenderTargets, int renderTargetCount, aga::Texture* depthStencil ) = 0;
	// virtual void BindRasterizerState( RE_HANDLE rasterizerState ) = 0;
	// virtual void BindSamplerState( SHADER_TYPE type, int startSlot, int numSamplers, const RE_HANDLE* pSamplerStates ) = 0;
	// virtual void BindDepthStencilState( RE_HANDLE depthStencilState ) = 0;
	// virtual void BindBlendState( RE_HANDLE blendState ) = 0;

	virtual void Draw( RESOURCE_PRIMITIVE primitive, UINT vertexCount, UINT vertexOffset = 0 ) = 0;
	virtual void DrawIndexed( RESOURCE_PRIMITIVE primitive, UINT indexCount, UINT indexOffset = 0, UINT vertexOffset = 0 ) = 0;
	virtual void DrawInstanced( RESOURCE_PRIMITIVE primitive, UINT vertexCount, UINT instanceCount, UINT vertexOffset = 0, UINT instanceOffset = 0 ) = 0;
	virtual void DrawInstancedInstanced( RESOURCE_PRIMITIVE primitive, UINT indexCount, UINT instanceCount, UINT indexOffset = 0, UINT vertexOffset = 0, UINT instanceOffset = 0 ) = 0;
	virtual void DrawAuto( ) = 0;
	virtual void Dispatch( UINT x, UINT y, UINT z = 1 ) = 0;

	virtual void Copy( aga::Buffer* dst, aga::Buffer* src, std::size_t size ) = 0;

	// virtual void GenerateMips( RE_HANDLE shaderResource ) = 0;

	virtual void GetRendererMultiSampleOption( MULTISAMPLE_OPTION* option ) = 0;

	virtual std::unique_ptr<aga::IImmediateCommandList> GetImmediateCommandList( ) const = 0;

	virtual ~IAga( ) = default;

protected:
	IAga( ) = default;
};

void CreateAbstractGraphicsApi( );
void DestoryAbstractGraphicsApi( );
