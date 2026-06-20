#pragma once

#include "Archive.h"
#include "ArchiveUtility.h"
#include "HashUtil.h"
#include "ShaderParameterMap.h"
#include "SizedTypes.h"

#include <array>
#include <ranges>
#include <vector>

namespace agl
{
	class ShaderParameterInfo
	{
	public:
		std::vector<ShaderParameter> m_cbvs;
		std::vector<ShaderParameter> m_globalCb;
		std::vector<ShaderParameter> m_srvs;
		std::vector<ShaderParameter> m_uavs;
		std::vector<ShaderParameter> m_samplers;
		std::vector<ShaderParameter> m_bindless;

		size_t GetHash() const
		{
			static size_t typeHash = typeid( ShaderParameterInfo ).hash_code();
			size_t hash = typeHash;

			for ( const auto& param : m_cbvs )
			{
				HashCombine( hash, param.GetHash() );
			}

			for ( const auto& param : m_globalCb )
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

		void Clear()
		{
			m_cbvs.clear();
			m_globalCb.clear();
			m_srvs.clear();
			m_uavs.clear();
			m_samplers.clear();
			m_bindless.clear();
		}

		void Merge( std::set<ShaderParameter>& OutShaderParameterSet ) const
		{
			for ( const auto& param : m_cbvs )
			{
				OutShaderParameterSet.emplace( param );
			}

			for ( const auto& param : m_globalCb )
			{
				OutShaderParameterSet.emplace( param );
			}

			for ( const auto& param : m_srvs )
			{
				OutShaderParameterSet.emplace( param );
			}

			for ( const auto& param : m_uavs )
			{
				OutShaderParameterSet.emplace( param );
			}

			for ( const auto& param : m_samplers )
			{
				OutShaderParameterSet.emplace( param );
			}

			for ( const auto& param : m_bindless )
			{
				OutShaderParameterSet.emplace( param );
			}
		}

		friend bool operator==( const ShaderParameterInfo& lhs, const ShaderParameterInfo& rhs )
		{
			return lhs.m_cbvs == rhs.m_cbvs
				&& lhs.m_globalCb == rhs.m_globalCb
				&& lhs.m_srvs == rhs.m_srvs 
				&& lhs.m_uavs == rhs.m_uavs 
				&& lhs.m_samplers == rhs.m_samplers 
				&& lhs.m_bindless == rhs.m_bindless;
		}

		friend Archive& operator<<( Archive& ar, ShaderParameterInfo& shaderParamInfo )
		{
			ar << shaderParamInfo.m_cbvs;
			ar << shaderParamInfo.m_globalCb;
			ar << shaderParamInfo.m_srvs;
			ar << shaderParamInfo.m_uavs;
			ar << shaderParamInfo.m_samplers;
			ar << shaderParamInfo.m_bindless;

			return ar;
		}
	};

	inline void AddShaderParameter( ShaderParameterInfo& outParameterInfo, const ShaderParameter& parameter )
	{
		const auto parameterType = parameter.m_type;
		if ( parameterType == ShaderParameterType::ConstantBuffer )
		{
			if ( parameter.m_bindPoint == 0 )
			{
				outParameterInfo.m_globalCb.emplace_back( parameter );
			}
			else
			{
				outParameterInfo.m_cbvs.emplace_back( parameter );
			}
		}
		else if ( parameterType == ShaderParameterType::ConstantBufferValue )
		{
			// Do Nothing
		}
		else if ( parameterType == ShaderParameterType::SRV )
		{
			outParameterInfo.m_srvs.emplace_back( parameter );
		}
		else if ( parameterType == ShaderParameterType::UAV )
		{
			outParameterInfo.m_uavs.emplace_back( parameter );
		}
		else if ( parameterType == ShaderParameterType::Sampler )
		{
			outParameterInfo.m_samplers.emplace_back( parameter );
		}
		else if ( parameterType == ShaderParameterType::Bindless )
		{
			outParameterInfo.m_bindless.emplace_back( parameter );
		}
		else
		{
			assert( false && "Invalid shader parameter type" );
		}
	}

	inline void BuildShaderParameterInfo( const std::map<Name, ShaderParameter>& parameterMap, ShaderParameterInfo& parameterInfo )
	{
		constexpr auto numShaderParameterType = static_cast<int32>( ShaderParameterType::Count );
		size_t numParameters[numShaderParameterType] = {};
		for ( const auto& shaderParameter : parameterMap | std::views::values )
		{
			auto typeIndex = static_cast<int32>( shaderParameter.m_type );
			++numParameters[typeIndex];
		}

		std::array<std::vector<ShaderParameter>*, numShaderParameterType> containers;
		containers[static_cast<int32>(ShaderParameterType::ConstantBuffer)] = &parameterInfo.m_cbvs;
		containers[static_cast<int32>(ShaderParameterType::ConstantBufferValue)] = nullptr;
		containers[static_cast<int32>(ShaderParameterType::SRV)] = &parameterInfo.m_srvs;
		containers[static_cast<int32>(ShaderParameterType::UAV)] = &parameterInfo.m_uavs;
		containers[static_cast<int32>(ShaderParameterType::Sampler)] = &parameterInfo.m_samplers;
		containers[static_cast<int32>(ShaderParameterType::Bindless)] = &parameterInfo.m_bindless;

		for ( int32 i = 0; i < numShaderParameterType; ++i )
		{
			if ( containers[i] == nullptr )
			{
				continue;
			}

			containers[i]->reserve( numParameters[i] );
		}

		for ( const auto& shaderParameter : parameterMap | std::views::values )
		{
			AddShaderParameter( parameterInfo, shaderParameter );
		}
	}
}