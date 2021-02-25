#pragma once

#include "IAga.h"
#include "DefaultConstantBuffers.h"
#include "ShaderPrameterMap.h"

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

private:
	IAga* m_aga = nullptr;
	DefaultConstantBuffers m_defaultConstants;
};

AgaDelegator& GetAgaDelegator();