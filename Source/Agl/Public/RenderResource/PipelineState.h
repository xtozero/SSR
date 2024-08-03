#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
#include "ShaderResource.h"
#include "SizedTypes.h"

namespace agl
{
	class BlendState : public GraphicsApiResource
	{
	public:
		AGL_DLL static RefHandle<BlendState> Create( const BlendStateTrait& trait );
	};

	class DepthStencilState : public GraphicsApiResource
	{
	public:
		AGL_DLL static RefHandle<DepthStencilState> Create( const DepthStencilStateTrait& trait );
	};

	class RasterizerState : public GraphicsApiResource
	{
	public:
		AGL_DLL static RefHandle<RasterizerState> Create( const RasterizerStateTrait& trait );
	};

	class SamplerState : public GraphicsApiResource
	{
	public:
		AGL_DLL static RefHandle<SamplerState> Create( const SamplerStateTrait& trait );
	};

	class VertexLayout : public GraphicsApiResource
	{
	public:
		AGL_DLL static RefHandle<VertexLayout> Create( const VertexShader* vs, const VertexLayoutTrait* trait, uint32 size );
	};

	class GraphicsPipelineStateInitializer final
	{
	public:
		friend bool operator<( const GraphicsPipelineStateInitializer& lhs, const GraphicsPipelineStateInitializer& rhs )
		{
			auto lVariable = std::tie( lhs.m_vertexShader, lhs.m_geometryShader, lhs.m_piexlShader, lhs.m_blendState, lhs.m_rasterizerState, lhs.m_depthStencilState, lhs.m_vertexLayout, lhs.m_primitiveType );
			auto rVariable = std::tie( rhs.m_vertexShader, rhs.m_geometryShader, rhs.m_piexlShader, rhs.m_blendState, rhs.m_rasterizerState, rhs.m_depthStencilState, rhs.m_vertexLayout, rhs.m_primitiveType );

			return lVariable < rVariable;
		}

		size_t GetHash() const;

		VertexShader* m_vertexShader = nullptr;
		GeometryShader* m_geometryShader = nullptr;
		PixelShader* m_piexlShader = nullptr;
		BlendState* m_blendState = nullptr;
		RasterizerState* m_rasterizerState = nullptr;
		DepthStencilState* m_depthStencilState = nullptr;
		VertexLayout* m_vertexLayout = nullptr;
		ResourcePrimitive m_primitiveType = ResourcePrimitive::Undefined;
	};

	class ComputePipelineStateInitializer final
	{
	public:
		friend bool operator<( const ComputePipelineStateInitializer& lhs, const ComputePipelineStateInitializer& rhs )
		{
			auto lVariable = std::tie( lhs.m_computeShader );
			auto rVariable = std::tie( rhs.m_computeShader );

			return lVariable < rVariable;
		}

		size_t GetHash() const;

		ComputeShader* m_computeShader = nullptr;
	};

	class GraphicsPipelineState : public GraphicsApiResource
	{
	public:
		AGL_DLL static RefHandle<GraphicsPipelineState> Create( const GraphicsPipelineStateInitializer& initializer );
	};

	class ComputePipelineState : public GraphicsApiResource
	{
	public:
		AGL_DLL static RefHandle<ComputePipelineState> Create( const ComputePipelineStateInitializer& initializer );
	};
}