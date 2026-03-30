#pragma once

#include "GraphicsApiResource.h"
#include "LibraryTool/Common.h"
#include "ShaderResource.h"
#include "SizedTypes.h"

namespace agl
{
	class BlendState : public GraphicsApiResource
	{
	public:
		AGL_DLL static RefHandle<BlendState> Create( const BlendStateDesc& desc );
	};

	class DepthStencilState : public GraphicsApiResource
	{
	public:
		AGL_DLL static RefHandle<DepthStencilState> Create( const DepthStencilStateDesc& desc );
	};

	class RasterizerState : public GraphicsApiResource
	{
	public:
		AGL_DLL static RefHandle<RasterizerState> Create( const RasterizerStateDesc& desc );
	};

	class SamplerState : public GraphicsApiResource
	{
	public:
		AGL_DLL static RefHandle<SamplerState> Create( const SamplerStateDesc& desc );
	};

	class VertexLayout : public GraphicsApiResource
	{
	public:
		AGL_DLL static RefHandle<VertexLayout> Create( const VertexShader* vs, const VertexLayoutData* layoutData, uint32 size );
	};

	class GraphicsPipelineStateDesc final
	{
	public:
		friend bool operator<( const GraphicsPipelineStateDesc& lhs, const GraphicsPipelineStateDesc& rhs )
		{
			auto lVariable = std::tie( lhs.m_vertexShader, lhs.m_geometryShader, lhs.m_piexlShader, lhs.m_meshShader, lhs.m_amplificationShader, lhs.m_blendState, lhs.m_rasterizerState, lhs.m_depthStencilState, lhs.m_vertexLayout, lhs.m_primitiveType );
			auto rVariable = std::tie( rhs.m_vertexShader, rhs.m_geometryShader, rhs.m_piexlShader, rhs.m_meshShader, rhs.m_amplificationShader, rhs.m_blendState, rhs.m_rasterizerState, rhs.m_depthStencilState, rhs.m_vertexLayout, rhs.m_primitiveType );

			return lVariable < rVariable;
		}

		size_t GetHash() const;

		VertexShader* m_vertexShader = nullptr;
		GeometryShader* m_geometryShader = nullptr;
		PixelShader* m_piexlShader = nullptr;
		MeshShader* m_meshShader = nullptr;
		AmplificationShader* m_amplificationShader = nullptr;
		BlendState* m_blendState = nullptr;
		RasterizerState* m_rasterizerState = nullptr;
		DepthStencilState* m_depthStencilState = nullptr;
		VertexLayout* m_vertexLayout = nullptr;
		ResourcePrimitive m_primitiveType = ResourcePrimitive::Undefined;
	};

	class ComputePipelineStateDesc final
	{
	public:
		friend bool operator<( const ComputePipelineStateDesc& lhs, const ComputePipelineStateDesc& rhs )
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
		AGL_DLL static RefHandle<GraphicsPipelineState> Create( const GraphicsPipelineStateDesc& desc );
	};

	class ComputePipelineState : public GraphicsApiResource
	{
	public:
		AGL_DLL static RefHandle<ComputePipelineState> Create( const ComputePipelineStateDesc& desc );
	};
}