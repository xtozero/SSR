#pragma once

#include "common.h"
#include "GraphicsApiResource.h"
#include "NameTypes.h"
#include "SizedTypes.h"

#include <map>

class Archive;

namespace aga
{
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
		uint32 m_bindPoint = 0;
		uint32 m_offset = 0;
		uint32 m_sizeInByte = 0;

		AGA_DLL size_t GetHash() const;
		AGA_DLL void Bind( const ShaderParameterMap& parameterMap, const char* variableName );

		friend AGA_DLL Archive& operator<<( Archive& ar, ShaderParameter& shaderParam );

		friend bool operator<( const ShaderParameter& lhs, const ShaderParameter& rhs )
		{
			auto lVariable = std::tie( lhs.m_shader, lhs.m_type, lhs.m_bindPoint, lhs.m_offset, lhs.m_sizeInByte );
			auto rVariable = std::tie( rhs.m_shader, rhs.m_type, rhs.m_bindPoint, rhs.m_offset, rhs.m_sizeInByte );

			return lVariable < rVariable;
		}

		friend bool operator==( const ShaderParameter& lhs, const ShaderParameter& rhs )
		{
			auto lVariable = std::tie( lhs.m_shader, lhs.m_type, lhs.m_bindPoint, lhs.m_offset, lhs.m_sizeInByte );
			auto rVariable = std::tie( rhs.m_shader, rhs.m_type, rhs.m_bindPoint, rhs.m_offset, rhs.m_sizeInByte );

			return lVariable == rVariable;
		}
	};

	class ShaderParameterMap
	{
	public:
		void AddParameter( const char* variableName, SHADER_TYPE shader, ShaderParameterType type, uint32 bindPoint, uint32 offset, uint32 sizeInByte )
		{
			m_parameters.emplace( variableName, ShaderParameter{ shader, type, bindPoint, offset, sizeInByte } );
		}

		void AddParameter( const char* variableName, const ShaderParameter& parameter )
		{
			m_parameters.emplace( variableName, parameter );
		}

		const std::map<Name, ShaderParameter>& GetParameterMap() const
		{
			return m_parameters;
		}

		std::map<Name, ShaderParameter>& GetParameterMap()
		{
			return m_parameters;
		}

		AGA_DLL ShaderParameter GetParameter( const char* name ) const;

		uint32 Size() const
		{
			return static_cast<uint32>( m_parameters.size() );
		}

		friend AGA_DLL Archive& operator<<( Archive& ar, ShaderParameterMap& shaderParamMap );

	private:
		std::map<Name, ShaderParameter> m_parameters;
	};
}
