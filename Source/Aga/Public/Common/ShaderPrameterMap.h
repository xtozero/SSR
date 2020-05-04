#pragma once

#include <map>

enum class ShaderParameterType
{
	ConstantBuffer,
	SRV,
	UAV,
	Sampler,
	Count
};

struct ShaderParameter
{
	ShaderParameterType m_type;
	UINT m_bindPoint;
};

class ShaderParameterMap
{
public:
	void AddParameter( const char* variableName, ShaderParameterType type, UINT bindPoint )
	{
		m_parameters.emplace( variableName, ShaderParameter{ type, bindPoint } );
	}

	const std::map<std::string, ShaderParameter> GetParameterMap( ) const
	{
		return m_parameters;
	}

private:
	std::map<std::string, ShaderParameter> m_parameters;
};