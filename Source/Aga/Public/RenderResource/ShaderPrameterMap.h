#pragma once

#include "common.h"

#include <map>

enum class ShaderParameterType
{
	Unknown,
	ConstantBuffer,
	ConstantBufferValue,
	SRV,
	UAV,
	Sampler,
	Count
};

class ShaderParameterMap;

class ShaderParameter
{
public:
	ShaderParameterType m_type = ShaderParameterType::Unknown;
	UINT m_bindPoint = 0;
	UINT m_offset = 0;

	AGA_DLL void Bind( const ShaderParameterMap& parameterMap, const char* variableName );
};

class ShaderParameterMap
{
public:
	void AddParameter( const char* variableName, ShaderParameterType type, UINT bindPoint, UINT offset )
	{
		m_parameters.emplace( variableName, ShaderParameter{ type, bindPoint, offset } );
	}

	const std::map<std::string, ShaderParameter>& GetParameterMap( ) const
	{
		return m_parameters;
	}

	ShaderParameter GetParameter( const char* name ) const;

	std::size_t Size( ) const
	{
		return m_parameters.size( );
	}

private:
	std::map<std::string, ShaderParameter> m_parameters;
};