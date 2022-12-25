#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
#include "ShaderResource.h"
#include "SizedTypes.h"

namespace agl
{
	class BlendState : public DeviceDependantResource
	{
	public:
		AGL_DLL static RefHandle<BlendState> Create( const BLEND_STATE_TRAIT& trait );
	};

	class DepthStencilState : public DeviceDependantResource
	{
	public:
		AGL_DLL static RefHandle<DepthStencilState> Create( const DEPTH_STENCIL_STATE_TRAIT& trait );
	};

	class RasterizerState : public DeviceDependantResource
	{
	public:
		AGL_DLL static RefHandle<RasterizerState> Create( const RASTERIZER_STATE_TRAIT& trait );
	};

	class SamplerState : public DeviceDependantResource
	{
	public:
		AGL_DLL static RefHandle<SamplerState> Create( const SAMPLER_STATE_TRAIT& trait );
	};

	class VertexLayout : public DeviceDependantResource
	{
	public:
		AGL_DLL static RefHandle<VertexLayout> Create( const VertexShader* vs, const VERTEX_LAYOUT_TRAIT* trait, uint32 size );
	};

	class GraphicsPipelineStateInitializer
	{
	public:
		friend bool operator<( const GraphicsPipelineStateInitializer& lhs, const GraphicsPipelineStateInitializer& rhs )
		{
			auto lVariable = std::tie( lhs.m_vertexShader, lhs.m_geometryShader, lhs.m_piexlShader, lhs.m_blendState, lhs.m_rasterizerState, lhs.m_depthStencilState, lhs.m_vertexLayout, lhs.m_primitiveType );
			auto rVariable = std::tie( rhs.m_vertexShader, rhs.m_geometryShader, rhs.m_piexlShader, rhs.m_blendState, rhs.m_rasterizerState, rhs.m_depthStencilState, rhs.m_vertexLayout, rhs.m_primitiveType );

			return lVariable < rVariable;
		}

		VertexShader* m_vertexShader = nullptr;
		GeometryShader* m_geometryShader = nullptr;
		PixelShader* m_piexlShader = nullptr;
		BlendState* m_blendState = nullptr;
		RasterizerState* m_rasterizerState = nullptr;
		DepthStencilState* m_depthStencilState = nullptr;
		VertexLayout* m_vertexLayout = nullptr;
		ResourcePrimitive m_primitiveType = ResourcePrimitive::Undefined;
	};

	class ComputePipelineStateInitializer
	{
	public:
		friend bool operator<( const ComputePipelineStateInitializer& lhs, const ComputePipelineStateInitializer& rhs )
		{
			auto lVariable = std::tie( lhs.m_computeShader );
			auto rVariable = std::tie( rhs.m_computeShader );

			return lVariable < rVariable;
		}

		ComputeShader* m_computeShader = nullptr;
	};

	class PipelineState : public DeviceDependantResource
	{
	public:
		AGL_DLL static RefHandle<PipelineState> Create( const GraphicsPipelineStateInitializer& initializer );
	};
}