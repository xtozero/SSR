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
	static GlobalShader& GetInstance()
	{
		static GlobalShader globalShader;
		return globalShader;
	}

	void BootUp();
	void Shutdown();
	bool IsReady() const;

	bool RegisterShader( std::type_index typeIndex, const std::shared_ptr<ShaderBase>& shader );
	bool RegisterShaderPath( std::type_index typeIndex, const char* path );

	IShader* GetShader( std::type_index typeIndex );

private:
	GlobalShader() = default;

	std::map<std::type_index, std::shared_ptr<IShader>> m_shaders;
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

IShader* GetGlobalShaderImpl( std::type_index typeIndex );

template <typename T>
IShader* GetGlobalShader()
{
	return GetGlobalShaderImpl( typeid( T ) );
}
