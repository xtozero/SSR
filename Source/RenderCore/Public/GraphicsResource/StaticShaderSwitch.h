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
	struct StaticShaderSwitch final
	{
		bool m_on;
		int32 m_current;
		int32 m_min;
		int32 m_max;
		int32 m_bias;

		uint32 GetID() const;
		RENDERCORE_DLL uint32 NumShaderValues() const;

		std::strong_ordering operator<=>( const StaticShaderSwitch& other ) const = default;
		
		friend RENDERCORE_DLL Archive& operator<<( Archive& ar, StaticShaderSwitch& shaderSwitch );
	};

	class StaticShaderSwitches final
	{
	public:
		RENDERCORE_DLL bool On( const Name& name, int32 value );
		RENDERCORE_DLL bool Off( const Name& name );
		RENDERCORE_DLL uint32 GetID() const;

		RENDERCORE_DLL void SetConfigs( const std::map<Name, StaticShaderSwitch>& configs );

		RENDERCORE_DLL std::map<Name, StaticShaderSwitch>& Configs();
		RENDERCORE_DLL const std::map<Name, StaticShaderSwitch>& Configs() const;

		std::strong_ordering operator<=>( const StaticShaderSwitches& other ) const = default;

		friend RENDERCORE_DLL Archive& operator<<( Archive& ar, StaticShaderSwitches& shaderSwitches );

	private:
		std::map<Name, StaticShaderSwitch> m_configs;
	};
}