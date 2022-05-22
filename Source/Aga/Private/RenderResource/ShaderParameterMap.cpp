#include "stdafx.h"
#include "ShaderParameterMap.h"

#include "Archive.h"
#include "ArchiveUtility.h"
#include "HashUtil.h"

namespace aga
{
	Archive& operator<<( Archive& ar, ShaderParameter& shaderParam )
	{
		ar << shaderParam.m_shader
			<< shaderParam.m_type
			<< shaderParam.m_bindPoint
			<< shaderParam.m_offset
			<< shaderParam.m_sizeInByte;

		return ar;
	}

	size_t ShaderParameter::GetHash() const
	{
		static size_t typeHash = typeid( ShaderParameter ).hash_code();
		size_t hash = typeHash;

		HashCombine( hash, m_shader );
		HashCombine( hash, m_type );
		HashCombine( hash, m_bindPoint );
		HashCombine( hash, m_offset );
		HashCombine( hash, m_sizeInByte );

		return hash;
	}

	void ShaderParameter::Bind( const ShaderParameterMap& parameterMap, const char* variableName )
	{
		( *this ) = parameterMap.GetParameter( variableName );
	}

	ShaderParameter ShaderParameterMap::GetParameter( const char* name ) const
	{
		auto found = m_parameters.find( Name( name ) );
		if ( found != m_parameters.end() )
		{
			return found->second;
		}

		return {};
	}

	Archive& operator<<( Archive& ar, ShaderParameterMap& shaderParamMap )
	{
		ar << shaderParamMap.GetParameterMap();

		return ar;
	}
}
