#pragma once

#include "IAga.h"
#include "DefaultConstantBuffers.h"
#include "GraphicsPipelineState.h"
#include "RenderOption.h"
#include "ShaderPrameterMap.h"
#include "SizedTypes.h"
#include "VertexLayout.h"

#include <unordered_map>

class ComputeShader;
class PixelShader;
class VertexShader;

class AbstractGraphicsInterface
{
public:
	void BootUp( aga::IAga* pAga );
	void Shutdown( );

	void* Lock( aga::Buffer* buffer, uint32 lockFlag = BUFFER_LOCKFLAG::WRITE_DISCARD, uint32 subResource = 0 );
	void UnLock( aga::Buffer* buffer, uint32 subResource = 0 );

	void SetViewports( aga::Viewport** pViewPorts, uint32 count );
	void SetViewport( uint32 minX, uint32 minY, float minZ, uint32 maxX, uint32 maxY, float maxZ );
	void SetScissorRects( aga::Viewport** pViewPorts, uint32 size );
	void SetScissorRect( uint32 minX, uint32 minY, uint32 maxX, uint32 maxY );

	template <typename ShaderType, typename ValueType>
	void SetShaderValue( ShaderType& shader, const aga::ShaderParameter& parameter, ValueType value )
	{
		assert( parameter.m_bindPoint == 0 );
		assert( parameter.m_type == aga::ShaderParameterType::ConstantBufferValue );
		m_defaultConstants.SetShaderValue( shader, parameter.m_offset, sizeof( ValueType ), &value );
	}
	
	template <typename ShaderType>
	void BindShader( ShaderType& shader )
	{
		m_aga->BindShader( shader.Resource( ) );
	}

	template <typename ShaderType>
	void BindConstant( ShaderType& shader, uint32 startSlot, uint32 numBuffers, aga::Buffer** pBuffers )
	{
		m_aga->BindConstant( shader.Resource( ), startSlot, numBuffers, pBuffers );
	}

	template <typename ShaderType>
	void BindShaderInput( ShaderType& shader, uint32 startSlot, uint32 numBuffers, aga::Buffer** pBuffers )
	{
		m_aga->BindShaderInput( shader.Resource( ), startSlot, numBuffers, pBuffers );
	}

	template <typename ShaderType>
	void BindShaderOutput( ShaderType& shader, uint32 startSlot, uint32 numBuffers, aga::Buffer** pBuffers )
	{
		m_aga->BindShaderOutput( shader.Resource( ), startSlot, numBuffers, pBuffers );
	}

	template <typename ShaderType>
	void BindShaderParameter( ShaderType& shader, const aga::ShaderParameter& parameter, aga::Buffer* buffer )
	{
		switch ( parameter.m_type )
		{
		case aga::ShaderParameterType::ConstantBuffer:
			BindConstant( shader, parameter.m_bindPoint, 1, &buffer );
			break;
		case aga::ShaderParameterType::SRV:
			BindShaderInput( shader, parameter.m_bindPoint, 1, &buffer );
			break;
		case aga::ShaderParameterType::Sampler:
			break;
		}
	}

	void BindShaderParameter( ComputeShader& shader, const aga::ShaderParameter& parameter, aga::Buffer* buffer )
	{
		switch ( parameter.m_type )
		{
		case aga::ShaderParameterType::ConstantBuffer:
			BindConstant( shader, parameter.m_bindPoint, 1, &buffer );
			break;
		case aga::ShaderParameterType::SRV:
			BindShaderInput( shader, parameter.m_bindPoint, 1, &buffer );
			break;
		case aga::ShaderParameterType::UAV:
			BindShaderOutput( shader, parameter.m_bindPoint, 1, &buffer );
			break;
		case aga::ShaderParameterType::Sampler:
			break;
		}
	}

	void BindRenderTargets( aga::Texture** pRenderTargets, uint32 renderTargetCount, aga::Texture* depthStencil );

	void ClearRenderTarget( aga::Texture* renderTarget, const float( &clearColor )[4] );
	void ClearDepthStencil( aga::Texture* depthStencil, float depthColor, UINT8 stencilColor );

	void Dispatch( uint32 x, uint32 y, uint32 z = 1 );

	void Copy( aga::Buffer* dst, aga::Buffer* src, uint32 size );

	BlendState FindOrCreate( const BlendOption& option );
	DepthStencilState FindOrCreate( const DepthStencilOption& option );
	RasterizerState FindOrCreate( const RasterizerOption& option );
	SamplerState FindOrCreate( const SamplerOption& option );
	VertexLayout FindOrCreate( const VertexShader& vs, const VertexLayoutDesc& desc );

private:
	aga::IAga* m_aga = nullptr;
	DefaultConstantBuffers m_defaultConstants;

	std::unordered_map<BlendOption, BlendState, BlendOptionHasher> m_blendStates;
	std::unordered_map<DepthStencilOption, DepthStencilState, DepthStencilOptionHasher> m_depthStencilStates;
	std::unordered_map<RasterizerOption, RasterizerState, RasterizerOptionHasher> m_rasterizerStates;
	std::unordered_map<SamplerOption, SamplerState, SamplerOptionHasher> m_samplerStates;
	std::unordered_map<VertexLayoutDesc, VertexLayout, VertexLayoutDescHasher> m_vertexLayouts;
};

AbstractGraphicsInterface& GraphicsInterface();