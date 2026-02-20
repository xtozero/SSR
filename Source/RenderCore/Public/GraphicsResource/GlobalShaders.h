#pragma once

#include "Shader.h"
#include "ShaderRegistry.h"
#include "SizedTypes.h"

#include <atomic>
#include <filesystem>
#include <map>
#include <typeindex>

namespace rendercore
{
	class GlobalShaders final
	{
	public:
		static GlobalShaders& GetInstance()
		{
			static GlobalShaders globalShaders;
			return globalShaders;
		}

		void BootUp();
		void Shutdown();
		bool IsReady() const;

		void Reload();

		bool RegisterShader( std::type_index typeIndex, const std::shared_ptr<ShaderAsset>& shader );
		bool RegisterShaderPath( std::type_index typeIndex, agl::ShaderType shaderType, const char* path );

		ShaderAsset* GetShader( std::type_index typeIndex );

	private:
		GlobalShaders() = default;

		std::map<std::type_index, std::shared_ptr<ShaderAsset>> m_shaders;
		std::map<std::type_index, std::filesystem::path> m_shaderAssetPaths;

		std::atomic<int32> m_loadingInProgress = 0;
	};

	class GlobalShaderRegister final
	{
	public:
		GlobalShaderRegister( std::type_index typeIndex, agl::ShaderType shaderType, const char* assetPath );
	};

	ShaderAsset* GetGlobalShaderImpl( std::type_index typeIndex );

	template <typename T>
	ShaderAsset* GetGlobalShader()
	{
		return GetGlobalShaderImpl( typeid( T ) );
	}

	template <typename ShaderClass, typename DerivedType>
	class GlobalShaderBase : public ShaderTraits<ShaderClass>
	{
	public:
		static const StaticShaderSwitches& GetSwitches()
		{
			static StaticShaderSwitches staticSwitch = GetGlobalShader<DerivedType>()->GetStaticSwitches();
			return staticSwitch;
		}

		GlobalShaderBase()
		{
			m_shader = GetGlobalShader<DerivedType>();
			assert( m_shader != nullptr );

			m_compiledShader = Cast<ShaderClass>( m_shader->CompileShader( {} ) );
			assert( m_compiledShader != nullptr );
		}

		explicit GlobalShaderBase( const StaticShaderSwitches& switches )
		{
			m_shader = GetGlobalShader<DerivedType>();
			assert( m_shader != nullptr );

			m_compiledShader = Cast<ShaderClass>( m_shader->CompileShader( switches ) );
			assert( m_compiledShader != nullptr );
		}

		virtual ~GlobalShaderBase() = default;
		GlobalShaderBase( const GlobalShaderBase& ) = default;
		GlobalShaderBase( GlobalShaderBase&& ) = default;
		GlobalShaderBase& operator=( const GlobalShaderBase& ) = default;
		GlobalShaderBase& operator=( GlobalShaderBase&& ) = default;

		operator ShaderClass* () const
		{
			return GetShader();
		}

	protected:
		ShaderClass* GetShader() const
		{
			return  Cast<ShaderClass>( m_compiledShader );
		}

		ShaderAsset* m_shader = nullptr;
		ShaderClass* m_compiledShader = nullptr;
	};

	#define REGISTER_GLOBAL_SHADER( shaderClass, shaderPath, entryPoint ) \
	REGISTER_SHADER( shaderClass, shaderPath, entryPoint ); \
	static GlobalShaderRegister shaderClass##_global_register( typeid( shaderClass ), shaderClass::Type, shaderPath )
}
