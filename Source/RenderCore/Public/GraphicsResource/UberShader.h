#pragma once

#include "NameTypes.h"
#include "Shader.h"

#include <memory>

namespace rendercore
{
	class UberShader : public IShader
	{
		GENERATE_CLASS_TYPE_INFO( UberShader );
		DECLARE_ASSET( RENDERCORE, UberShader );

	public:
		friend class ShaderManufacturer;
		
		RENDERCORE_DLL virtual ShaderBase* CompileShader( const StaticShaderSwitches& switches ) override;
		const StaticShaderSwitches& Switches() const;

		friend bool operator==( const UberShader& lhs, const UberShader& rhs )
		{
			return lhs.m_type == rhs.m_type
				&& lhs.m_profile == rhs.m_profile
				&& lhs.m_shaderCode == rhs.m_shaderCode
				&& lhs.m_switches == rhs.m_switches
				&& lhs.m_compiledShader == rhs.m_compiledShader;
		}

	protected:
		RENDERCORE_DLL virtual void PostLoadImpl() override;

	private:
		PROPERTY( type )
		SHADER_TYPE m_type;

		PROPERTY( profile )
		Name m_profile;

		PROPERTY( shaderCode )
		BinaryChunk m_shaderCode{ 0 };

		PROPERTY( switches )
		StaticShaderSwitches m_switches;

		std::map<uint32, std::unique_ptr<ShaderBase>> m_compiledShader;
	};
}