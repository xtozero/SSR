#include "stdafx.h"
#include "ShaderPrameterMap.h"

namespace aga
{
	void ShaderParameter::Bind( const ShaderParameterMap& parameterMap, const char* variableName )
	{
		( *this ) = parameterMap.GetParameter( variableName );
	}

	ShaderParameter ShaderParameterMap::GetParameter( const char* name ) const
	{
		auto found = m_parameters.find( name );
		if ( found != m_parameters.end( ) )
		{
			return found->second;
		}

		return {};
	}
}
