#include "ShaderParameterMap.h"

#include "Archive.h"
#include "ArchiveUtility.h"
#include "HashUtil.h"

namespace agl
{
	Archive& operator<<( Archive& ar, ShaderParameter& shaderParam )
	{
		ar << shaderParam.m_shader
			<< shaderParam.m_type
			<< shaderParam.m_bindPoint
			<< shaderParam.m_space
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
		HashCombine( hash, m_space );
		HashCombine( hash, m_offset );
		HashCombine( hash, m_sizeInByte );

		return hash;
	}

	void ShaderParameter::Bind( const ShaderParameterMap& parameterMap, Name variableName )
	{
		( *this ) = parameterMap.GetParameter( variableName );
	}

	ShaderParameter::ShaderParameter( ShaderType shader, ShaderParameterType type, uint32 bindPoint, uint32 space, uint32 offset, uint32 sizeInByte )
		: m_shader( shader )
		, m_type( type )
		, m_bindPoint( bindPoint )
		, m_space( space )
		, m_offset( offset )
		, m_sizeInByte( sizeInByte )
	{}

	ShaderParameter::ShaderParameter( const ShaderParameterMap& parameterMap, const char* variableName )
	{
		Bind( parameterMap, Name( variableName ) );
	}

	ShaderParameter ShaderParameterMap::GetParameter( Name name ) const
	{
		auto found = m_parameters.find( name );
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
