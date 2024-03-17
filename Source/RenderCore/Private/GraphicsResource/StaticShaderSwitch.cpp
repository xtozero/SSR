#include "StaticShaderSwitch.h"

#include "Archive.h"
#include "ArchiveUtility.h"

namespace rendercore
{
	uint32 StaticShaderSwitch::GetID() const
	{
		if ( m_on == false )
		{
			return 0;
		}

		return std::abs( m_current - m_min ) + 1;
	}

	uint32 StaticShaderSwitch::NumShaderValues() const
	{
		return std::abs( m_max - m_min ) + 2;
	}

	Archive& operator<<( Archive& ar, StaticShaderSwitch& shaderSwitch )
	{
		ar << shaderSwitch.m_on << shaderSwitch.m_current << shaderSwitch.m_min << shaderSwitch.m_max << shaderSwitch.m_bias;
		return ar;
	}

	bool StaticShaderSwitches::On( const Name& name, int32 value )
	{
		auto found = m_configs.find( name );
		if ( found == std::end( m_configs ) )
		{
			return false;
		}

		StaticShaderSwitch& shaderSwitch = found->second;
		shaderSwitch.m_on = true;
		shaderSwitch.m_current = std::clamp( value, shaderSwitch.m_min, shaderSwitch.m_max );
		return true;
	}

	bool StaticShaderSwitches::Off( const Name& name )
	{
		auto found = m_configs.find( name );
		if ( found == std::end( m_configs ) )
		{
			return false;
		}

		StaticShaderSwitch& shaderSwitch = found->second;
		shaderSwitch.m_on = false;
		return true;
	}

	uint32 StaticShaderSwitches::GetID() const
	{
		uint32 id = 0;
		
		for ( auto& [name, shaderSwitch] : m_configs )
		{
			id += shaderSwitch.GetID() * shaderSwitch.m_bias;
		}

		return id;
	}

	void StaticShaderSwitches::SetConfigs( const std::map<Name, StaticShaderSwitch>& configs )
	{
		m_configs = configs;
	}

	std::map<Name, StaticShaderSwitch>& StaticShaderSwitches::Configs()
	{
		return m_configs;
	}

	const std::map<Name, StaticShaderSwitch>& StaticShaderSwitches::Configs() const
	{
		return m_configs;
	}

	Archive& operator<<( Archive& ar, StaticShaderSwitches& shaderSwitches )
	{
		ar << shaderSwitches.m_configs;
		return ar;
	}
}
