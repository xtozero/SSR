#pragma once
#include "Shader.h"

#include <atomic>
#include <map>
#include <string>
#include <typeindex>

class GlobalShader
{
public:
	static GlobalShader& GetInstance( )
	{
		static GlobalShader globalShader;
		return globalShader;
	}

	bool RegisterShader( std::type_index typeIndex, const std::shared_ptr<ShaderBase>& shader );

	void OnLoadShaderStarted( std::type_index typeIndex );

	ShaderBase* GetShader( std::type_index typeIndex );

private:
	GlobalShader( ) = default;

	std::map<std::type_index, std::shared_ptr<ShaderBase>> m_shaders;

	std::atomic<int> m_loadingInProgress = 0;
};

class GlobalShaderRegister
{
public:
	GlobalShaderRegister( std::type_index typeIndex, const std::string& assetPath );
};

#define REGISTER_GLOBAL_SHADER( type, shaderPath ) \
GlobalShaderRegister type##_register( typeid( type ), shaderPath );

ShaderBase* GetGlobalShaderImpl( std::type_index typeIndex );

template <typename T>
ShaderBase* GetGlobalShader( )
{
	return GetGlobalShaderImpl( typeid( T ) );
}
