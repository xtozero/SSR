#pragma once

#include "IAga.h"
#include "DefaultConstantBuffers.h"
#include "GraphicsPipelineState.h"
#include "RenderOption.h"
#include "ShaderPrameterMap.h"

#include <unordered_map>

class ComputeShader;
class PixelShader;
class VertexShader;

class AgaDelegator
{
public:
	void BootUp( IAga* pAga );
	void Shutdown( );

	template <typename ShaderType, typename ValueType>
	void SetShaderValue( ShaderType& shader, const ShaderParameter& parameter, ValueType value )
	{
		assert( parameter.m_bindPoint == 0 );

		m_defaultConstants.SetShaderValue( shader, parameter.m_offset, sizeof( ValueType ), &value );
	}
	
	template <typename ShaderType>
	void BindShader( ShaderType& shader )
	{
		m_aga->BindShader( shader );
	}

	void Dispatch( UINT x, UINT y, UINT z = 1 );

	BlendState FindOrCreate( const BlendOption& option );
	DepthStencilState FindOrCreate( const DepthStencilOption& option );
	RasterizerState FindOrCreate( const RasterizerOption& option );
	SamplerState FindOrCreate( const SamplerOption& option );

private:
	IAga* m_aga = nullptr;
	DefaultConstantBuffers m_defaultConstants;

	std::unordered_map<BlendOption, BlendState, BlendOptionHasher> m_blendStates;
	std::unordered_map<DepthStencilOption, DepthStencilState, DepthStencilOptionHasher> m_depthStencilStates;
	std::unordered_map<RasterizerOption, RasterizerState, RasterizerOptionHasher> m_rasterizerStates;
	std::unordered_map<SamplerOption, SamplerState, SamplerOptionHasher> m_samplerStates;
};

AgaDelegator& GetAgaDelegator();