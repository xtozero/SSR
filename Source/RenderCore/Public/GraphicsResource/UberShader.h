#pragma once

#include "NameTypes.h"
#include "Shader.h"

#include <memory>
#include <set>

namespace rendercore
{
	class UberShader final : public ShaderAsset
	{
		GENERATE_CLASS_TYPE_INFO( UberShader );
		DECLARE_ASSET( RENDERCORE, UberShader );

	public:
		RENDERCORE_DLL virtual ShaderBase* CompileShader( const IShaderPermutation& permutation ) override;
		RENDERCORE_DLL virtual void RecompileShader() override;

		RENDERCORE_DLL virtual agl::ShaderParameterMap& ParameterMap() override;
		RENDERCORE_DLL virtual const agl::ShaderParameterMap& ParameterMap() const override;

		RENDERCORE_DLL virtual agl::ShaderParameterInfo& ParameterInfo() override;
		RENDERCORE_DLL virtual const agl::ShaderParameterInfo& ParameterInfo() const override;

		RENDERCORE_DLL void SetName( const std::string& name );

		RENDERCORE_DLL void SetShaderType( agl::ShaderType type );

		RENDERCORE_DLL void SetEntryPoint( const std::string& entryPoint );

		RENDERCORE_DLL void SetShaderCode( const std::string& shaderCode );

		RENDERCORE_DLL void SetShaderDescriptorHandle( uint32 handle );

		BinaryChunk ComipeShaderByteCode( const IShaderPermutation& permutation ) const;

		friend bool operator==( const UberShader& lhs, const UberShader& rhs )
		{
			return lhs.m_type == rhs.m_type
				&& lhs.m_shaderCode == rhs.m_shaderCode;
		}

	protected:
		RENDERCORE_DLL virtual void PostLoadImpl() override;

	private:
		PROPERTY( name )
		std::string m_name;

		PROPERTY( type )
		agl::ShaderType m_type = agl::ShaderType::None;

		PROPERTY( entryPoint )
		std::string m_entryPoint;

		PROPERTY( shaderCode )
		BinaryChunk m_shaderCode{ 0 };

		PROPERTY( shaderDescriptorHandle )
		uint32 m_shaderDescriptorHandle = 0;

		static agl::ShaderParameterMap m_emptyParameterMap;
		static agl::ShaderParameterInfo m_emptyParameterInfo;
	};
}