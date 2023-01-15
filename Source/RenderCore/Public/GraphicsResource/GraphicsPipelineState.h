#pragma once

#include "GraphicsApiResource.h"
#include "PipelineState.h"
#include "ShaderBindings.h"
#include "VertexLayout.h"

namespace rendercore
{
	class BlendState
	{
	public:
		agl::BlendState* Resource();
		const agl::BlendState* Resource() const;

		explicit BlendState( const agl::BLEND_STATE_TRAIT& trait );

		BlendState() = default;
		~BlendState() = default;
		BlendState( const BlendState& ) = default;
		BlendState& operator=( const BlendState& ) = default;
		BlendState( BlendState&& ) = default;
		BlendState& operator=( BlendState&& ) = default;

	private:
		void InitResource( const agl::BLEND_STATE_TRAIT& trait );

		agl::RefHandle<agl::BlendState> m_state;
	};

	class DepthStencilState
	{
	public:
		agl::DepthStencilState* Resource();
		const agl::DepthStencilState* Resource() const;

		explicit DepthStencilState( const agl::DEPTH_STENCIL_STATE_TRAIT& trait );

		DepthStencilState() = default;
		~DepthStencilState() = default;
		DepthStencilState( const DepthStencilState& ) = default;
		DepthStencilState& operator=( const DepthStencilState& ) = default;
		DepthStencilState( DepthStencilState&& ) = default;
		DepthStencilState& operator=( DepthStencilState&& ) = default;

	private:
		void InitResource( const agl::DEPTH_STENCIL_STATE_TRAIT& trait );

		agl::RefHandle<agl::DepthStencilState> m_state;
	};

	class RasterizerState
	{
	public:
		agl::RasterizerState* Resource();
		const agl::RasterizerState* Resource() const;

		explicit RasterizerState( const agl::RASTERIZER_STATE_TRAIT& trait );

		RasterizerState() = default;
		~RasterizerState() = default;
		RasterizerState( const RasterizerState& ) = default;
		RasterizerState& operator=( const RasterizerState& ) = default;
		RasterizerState( RasterizerState&& ) = default;
		RasterizerState& operator=( RasterizerState&& ) = default;

	private:
		void InitResource( const agl::RASTERIZER_STATE_TRAIT& trait );

		agl::RefHandle<agl::RasterizerState> m_state;
	};

	class SamplerState
	{
	public:
		agl::SamplerState* Resource();
		const agl::SamplerState* Resource() const;

		explicit SamplerState( const agl::SAMPLER_STATE_TRAIT& trait );

		SamplerState() = default;
		~SamplerState() = default;
		SamplerState( const SamplerState& ) = default;
		SamplerState& operator=( const SamplerState& ) = default;
		SamplerState( SamplerState&& ) = default;
		SamplerState& operator=( SamplerState&& ) = default;

	private:
		void InitResource( const agl::SAMPLER_STATE_TRAIT& trait );

		agl::RefHandle<agl::SamplerState> m_state;
	};

	struct ShaderStates
	{
		VertexLayout m_vertexLayout;
		VertexShader* m_vertexShader = nullptr;
		// Reserved
		// HullShadaer m_hullShader;
		// DomainShader m_domainShader;
		GeometryShader* m_geometryShader = nullptr;
		PixelShader* m_pixelShader = nullptr;
	};

	inline agl::ShaderBindingsInitializer CreateShaderBindingsInitializer( const ShaderStates& state )
	{
		agl::ShaderBindingsInitializer initializer;

		if ( state.m_vertexShader && state.m_vertexShader->IsValid() )
		{
			initializer[agl::ShaderType::VS] = &state.m_vertexShader->ParameterInfo();
		}

		if ( state.m_geometryShader && state.m_geometryShader->IsValid() )
		{
			initializer[agl::ShaderType::GS] = &state.m_geometryShader->ParameterInfo();
		}

		if ( state.m_pixelShader && state.m_pixelShader->IsValid() )
		{
			initializer[agl::ShaderType::PS] = &state.m_pixelShader->ParameterInfo();
		}

		return initializer;
	}

	struct GraphicsPipelineState
	{
		ShaderStates m_shaderState;
		RasterizerState m_rasterizerState;
		DepthStencilState m_depthStencilState;
		BlendState m_blendState;
		agl::ResourcePrimitive m_primitive;

		agl::RefHandle<agl::GraphicsPipelineState> m_pso;
	};
}
