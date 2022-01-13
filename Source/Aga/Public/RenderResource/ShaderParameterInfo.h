#pragma once

#include "HashUtil.h"
#include "ShaderParameterMap.h"
#include "SizedTypes.h"

#include <vector>

namespace aga
{
	class ShaderParameterInfo
	{
	public:
		std::vector<ShaderParameter> m_constantBuffers;
		std::vector<ShaderParameter> m_srvs;
		std::vector<ShaderParameter> m_uavs;
		std::vector<ShaderParameter> m_samplers;

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

			return hash;
		}

		friend bool operator==( const ShaderParameterInfo& lhs, const ShaderParameterInfo& rhs )
		{
			return lhs.m_constantBuffers == rhs.m_constantBuffers &&
				lhs.m_srvs == rhs.m_srvs &&
				lhs.m_uavs == rhs.m_uavs &&
				lhs.m_samplers == rhs.m_samplers;
		}
	};
}