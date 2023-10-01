#pragma once

#include "NameTypes.h"
#include "Shader.h"

#include <memory>
#include <set>

namespace rendercore
{
	class UberShader : public IShader
	{
		GENERATE_CLASS_TYPE_INFO( UberShader );
		DECLARE_ASSET( RENDERCORE, UberShader );

	public:
		friend class ShaderManufacturer;
		
		RENDERCORE_DLL virtual StaticShaderSwitches GetStaticSwitches() const override;
		RENDERCORE_DLL virtual ShaderBase* CompileShader( const StaticShaderSwitches& switches ) override;

		RENDERCORE_DLL virtual agl::ShaderParameterMap& ParameterMap() override;
		RENDERCORE_DLL virtual const agl::ShaderParameterMap& ParameterMap() const override;

		RENDERCORE_DLL virtual agl::ShaderParameterInfo& ParameterInfo() override;
		RENDERCORE_DLL virtual const agl::ShaderParameterInfo& ParameterInfo() const override;

		friend bool operator==( const UberShader& lhs, const UberShader& rhs )
		{
			return lhs.m_type == rhs.m_type
				&& lhs.m_profile == rhs.m_profile
				&& lhs.m_shaderCode == rhs.m_shaderCode
				&& lhs.m_switches == rhs.m_switches
				&& lhs.m_validVariation == rhs.m_validVariation;
		}

	protected:
		RENDERCORE_DLL virtual void PostLoadImpl() override;

	private:
		PROPERTY( name )
		std::string m_name;

		PROPERTY( type )
		agl::ShaderType m_type = agl::ShaderType::None;

		PROPERTY( profile )
		Name m_profile;

		PROPERTY( shaderCode )
		BinaryChunk m_shaderCode{ 0 };

		PROPERTY( switches )
		StaticShaderSwitches m_switches;

		PROPERTY( validVariation )
		std::set<uint32> m_validVariation;

		static agl::ShaderParameterMap m_emptyParameterMap;
		static agl::ShaderParameterInfo m_emptyParameterInfo;
	};
}