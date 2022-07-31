#pragma once

#include "Common.h"
#include "NameTypes.h"
#include "SizedTypes.h"

#include <compare>
#include <map>
#include <string>

class Archive;

namespace rendercore
{
	struct StaticShaderSwitch
	{
		bool m_on;
		int32 m_current;
		int32 m_min;
		int32 m_max;

		uint32 GetID() const;
		uint32 NumShaderValues() const;

		std::strong_ordering operator<=>( const StaticShaderSwitch& other ) const = default;
		
		friend RENDERCORE_DLL Archive& operator<<( Archive& ar, StaticShaderSwitch& shaderSwitch );
	};

	class StaticShaderSwitches
	{
	public:
		RENDERCORE_DLL bool On( const Name& name, int32 value );
		RENDERCORE_DLL bool Off( const Name& name );
		RENDERCORE_DLL uint32 GetID() const;

		RENDERCORE_DLL const std::map<Name, StaticShaderSwitch>& Configs() const;

		std::strong_ordering operator<=>( const StaticShaderSwitches& other ) const = default;

		friend RENDERCORE_DLL Archive& operator<<( Archive& ar, StaticShaderSwitches& shaderSwitches );

		friend class ShaderManufacturer;

	private:
		std::map<Name, StaticShaderSwitch> m_configs;
	};
}