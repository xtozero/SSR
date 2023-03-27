#pragma once

#include "GraphicsApiResource.h"
#include "PipelineState.h"
#include "ResourceViews.h"

#include <d3d11.h>

namespace agl
{
	class Buffer;
	class ShaderBindings;
	class Texture;

	class D3D11PipelineCache final
	{
	public:
		void BindVertexBuffer( ID3D11DeviceContext& context, Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets );
		void BindIndexBuffer( ID3D11DeviceContext& context, Buffer* indexBuffer, uint32 indexOffset );
		void BindPipelineState( ID3D11DeviceContext& context, GraphicsPipelineState* pipelineState );
		void BindPipelineState( ID3D11DeviceContext& context, ComputePipelineState* pipelineState );
		void BindShader( ID3D11DeviceContext& context, VertexShader* vs );
		void BindShader( ID3D11DeviceContext& context, GeometryShader* gs );
		void BindShader( ID3D11DeviceContext& context, PixelShader* ps );
		void BindShader( ID3D11DeviceContext& context, ComputeShader* cs );
		void BindShaderResources( ID3D11DeviceContext& context, const ShaderBindings& shaderBindings );
		void SetViewports( ID3D11DeviceContext& context, uint32 count, const CubeArea<float>* area );
		void SetScissorRects( ID3D11DeviceContext& context, uint32 count, const RectangleArea<int32>* area );
		void BindRenderTargets( ID3D11DeviceContext& context, RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil );

	private:
		void UnbindExistingSRV( ID3D11DeviceContext& context, ID3D11ShaderResourceView* srv );
		void UnbindExistingUAV( ID3D11DeviceContext& context, ID3D11UnorderedAccessView* uav );
		void BindConstantBuffer( ID3D11DeviceContext& context, ShaderType shader, uint32 slot, Buffer* cb );
		void BindSRV( ID3D11DeviceContext& context, ShaderType shader, uint32 slot, ShaderResourceView* srv );
		void BindUAV( ID3D11DeviceContext& context, ShaderType shader, uint32 slot, UnorderedAccessView* uav );
		void BindSampler( ID3D11DeviceContext& context, ShaderType shader, uint32 slot, SamplerState* sampler );

		ID3D11VertexShader* m_vertexShader = nullptr;
		ID3D11GeometryShader* m_geometryShader = nullptr;
		ID3D11PixelShader* m_pixelShader = nullptr;
		ID3D11ComputeShader* m_computeShader = nullptr;

		D3D11_PRIMITIVE_TOPOLOGY m_primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
		ID3D11InputLayout* m_inputLayout = nullptr;
		ID3D11BlendState* m_blendState = nullptr;
		uint32 m_blendSampleMask = 0;
		ID3D11RasterizerState* m_rasterizerState = nullptr;
		ID3D11DepthStencilState* m_depthStencilState = nullptr;
		ID3D11SamplerState* m_samplerStates[MAX_SHADER_TYPE<uint32>][D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT] = {};

		ID3D11ShaderResourceView* m_srvs[MAX_SHADER_TYPE<uint32>][D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT] = {};
		ID3D11UnorderedAccessView* m_uavs[D3D11_PS_CS_UAV_REGISTER_COUNT] = {};
		ID3D11RenderTargetView* m_rtvs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
		ID3D11DepthStencilView* m_dsv = nullptr;

		ID3D11Buffer* m_vertexBuffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};
		uint32 m_vertexStrides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};
		uint32 m_vertexOffsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT] = {};

		ID3D11Buffer* m_indexBuffer = nullptr;
		DXGI_FORMAT m_indexBufferFormat = DXGI_FORMAT_UNKNOWN;

		ID3D11Buffer* m_constantBuffers[MAX_SHADER_TYPE<uint32>][D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT] = {};

		D3D11_VIEWPORT m_viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE] = {};

		D3D11_RECT m_siccorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE] = {};
	};
}