#pragma once

#include "Shader.h"
#include "SizedTypes.h"

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

	void BootUp( );

	bool RegisterShader( std::type_index typeIndex, const std::shared_ptr<ShaderBase>& shader );
	bool RegisterShaderPath( std::type_index typeIndex, const char* path );

	ShaderBase* GetShader( std::type_index typeIndex );

private:
	GlobalShader( ) = default;

	std::map<std::type_index, std::shared_ptr<ShaderBase>> m_shaders;
	std::map<std::type_index, const char*> m_shaderAssetPaths;

	std::atomic<int32> m_loadingInProgress = 0;
};

class GlobalShaderRegister
{
public:
	GlobalShaderRegister( std::type_index typeIndex, const char* assetPath );
};

#define REGISTER_GLOBAL_SHADER( type, shaderPath ) \
GlobalShaderRegister type##_register( typeid( type ), shaderPath );

ShaderBase* GetGlobalShaderImpl( std::type_index typeIndex );

template <typename T>
ShaderBase* GetGlobalShader( )
{
	return GetGlobalShaderImpl( typeid( T ) );
}
