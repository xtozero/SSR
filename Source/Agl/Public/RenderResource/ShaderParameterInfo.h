#pragma once

#include "Archive.h"
#include "ArchiveUtility.h"
#include "HashUtil.h"
#include "ShaderParameterMap.h"
#include "SizedTypes.h"

#include <vector>

namespace agl
{
	class ShaderParameterInfo
	{
	public:
		std::vector<ShaderParameter> m_constantBuffers;
		std::vector<ShaderParameter> m_srvs;
		std::vector<ShaderParameter> m_uavs;
		std::vector<ShaderParameter> m_samplers;
		std::vector<ShaderParameter> m_bindless;

		size_t GetHash() const
		{
			static size_t typeHash = typeid( ShaderParameterInfo ).hash_code();
			size_t hash = typeHash;

			for ( const auto& param : m_constantBuffers )
			{
				HashCombine( hash, param.GetHash() );
			}

			for ( const auto& param : m_srvs )
			{
				HashCombine( hash, param.GetHash() );
			}

			for ( const auto& param : m_uavs )
			{
				HashCombine( hash, param.GetHash() );
			}

			for ( const auto& param : m_samplers )
			{
				HashCombine( hash, param.GetHash() );
			}

			for ( const auto& param : m_bindless )
			{
				HashCombine( hash, param.GetHash() );
			}

			return hash;
		}

		friend bool operator==( const ShaderParameterInfo& lhs, const ShaderParameterInfo& rhs )
		{
			return lhs.m_constantBuffers == rhs.m_constantBuffers 
				&& lhs.m_srvs == rhs.m_srvs 
				&& lhs.m_uavs == rhs.m_uavs 
				&& lhs.m_samplers == rhs.m_samplers 
				&& lhs.m_bindless == rhs.m_bindless;
		}

		friend Archive& operator<<( Archive& ar, ShaderParameterInfo& shaderParamInfo )
		{
			ar << shaderParamInfo.m_constantBuffers;
			ar << shaderParamInfo.m_srvs;
			ar << shaderParamInfo.m_uavs;
			ar << shaderParamInfo.m_samplers;
			ar << shaderParamInfo.m_bindless;

			return ar;
		}
	};

	inline void BuildShaderParameterInfo( const std::map<Name, ShaderParameter>& parameterMap, ShaderParameterInfo& parameterInfo )
	{
		for ( uint32 i = 0; i < static_cast<uint32>( ShaderParameterType::Count ); ++i )
		{
			auto curParameterType = static_cast<ShaderParameterType>( i );
			size_t count = 0;

			for ( auto iter = parameterMap.begin(); iter != parameterMap.end(); ++iter )
			{
				if ( iter->second.m_type == curParameterType )
				{
					++count;
				}
			}

			std::vector<ShaderParameter>* shaderParameters = nullptr;

			if ( curParameterType == ShaderParameterType::ConstantBuffer )
			{
				shaderParameters = &parameterInfo.m_constantBuffers;
			}
			else if ( curParameterType == ShaderParameterType::SRV )
			{
				shaderParameters = &parameterInfo.m_srvs;
			}
			else if ( curParameterType == ShaderParameterType::UAV )
			{
				shaderParameters = &parameterInfo.m_uavs;
			}
			else if ( curParameterType == ShaderParameterType::Sampler )
			{
				shaderParameters = &parameterInfo.m_samplers;
			}
			else if ( curParameterType == ShaderParameterType::Bindless )
			{
				shaderParameters = &parameterInfo.m_bindless;
			}
			else
			{
				continue;
			}

			shaderParameters->reserve( count );

			for ( auto iter = parameterMap.begin(); iter != parameterMap.end(); ++iter )
			{
				if ( iter->second.m_type == curParameterType )
				{
					shaderParameters->push_back( iter->second );
				}
			}
		}
	}
}