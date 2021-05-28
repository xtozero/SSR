#pragma once

#include "common.h"
#include "GraphicsApiResource.h"

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
	SHADER_TYPE m_shader = SHADER_TYPE::NONE;
	ShaderParameterType m_type = ShaderParameterType::Unknown;
	UINT m_bindPoint = 0;
	UINT m_offset = 0;
	UINT m_sizeInByte = 0;

	AGA_DLL void Bind( const ShaderParameterMap& parameterMap, const char* variableName );

	friend bool operator<( const ShaderParameter& lhs, const ShaderParameter& rhs )
	{
		auto lVariable = std::tie( lhs.m_shader, lhs.m_type, lhs.m_bindPoint, lhs.m_offset, lhs.m_sizeInByte );
		auto rVariable = std::tie( rhs.m_shader, rhs.m_type, rhs.m_bindPoint, rhs.m_offset, rhs.m_sizeInByte );

		return lVariable < rVariable;
	}
};

class ShaderParameterMap
{
public:
	void AddParameter( const char* variableName, SHADER_TYPE shader, ShaderParameterType type, UINT bindPoint, UINT offset, UINT sizeInByte )
	{
		m_parameters.emplace( variableName, ShaderParameter{ shader, type, bindPoint, offset, sizeInByte } );
	}

	void AddParameter( const char* variableName, const ShaderParameter& parameter )
	{
		m_parameters.emplace( variableName, parameter );
	}

	const std::map<std::string, ShaderParameter>& GetParameterMap( ) const
	{
		return m_parameters;
	}

	std::map<std::string, ShaderParameter>& GetParameterMap( )
	{
		return m_parameters;
	}

	AGA_DLL ShaderParameter GetParameter( const char* name ) const;

	std::size_t Size( ) const
	{
		return m_parameters.size( );
	}

private:
	std::map<std::string, ShaderParameter> m_parameters;
};