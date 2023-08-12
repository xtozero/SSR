#pragma once

#include "Shader.h"
#include "SizedTypes.h"

#include <atomic>
#include <map>
#include <string>
#include <typeindex>
#include <type_traits>

namespace rendercore
{
	class GlobalShaders
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

		bool RegisterShader( std::type_index typeIndex, const std::shared_ptr<IShader>& shader );
		bool RegisterShaderPath( std::type_index typeIndex, const char* path );

		IShader* GetShader( std::type_index typeIndex );

	private:
		GlobalShaders() = default;

		std::map<std::type_index, std::shared_ptr<IShader>> m_shaders;
		std::map<std::type_index, const char*> m_shaderAssetPaths;

		std::atomic<int32> m_loadingInProgress = 0;
	};

	class GlobalShaderRegister
	{
	public:
		GlobalShaderRegister( std::type_index typeIndex, const char* assetPath );
	};

	IShader* GetGlobalShaderImpl( std::type_index typeIndex );

	template <typename T>
	IShader* GetGlobalShader()
	{
		return GetGlobalShaderImpl( typeid( T ) );
	}

	class GlobalShaderBase
	{
	public:
		virtual ~GlobalShaderBase() = default;
	};

	template <typename ShaderType, typename DerivedType>
	class GlobalShaderCommon : public GlobalShaderBase
	{
	public:
		static const StaticShaderSwitches& GetSwitches()
		{
			static StaticShaderSwitches staticSwitch = GetGlobalShader<DerivedType>()->GetStaticSwitches();
			return staticSwitch;
		}

		GlobalShaderCommon()
		{
			m_shader = GetGlobalShader<DerivedType>();
			assert( m_shader != nullptr );

			m_compiledShader = Cast<ShaderType>( m_shader->CompileShader( {} ) );
			assert( m_compiledShader != nullptr );
		}

		GlobalShaderCommon( const StaticShaderSwitches& switches )
		{
			m_shader = GetGlobalShader<DerivedType>();
			assert( m_shader != nullptr );

			m_compiledShader = Cast<ShaderType>( m_shader->CompileShader( switches ) );
			assert( m_compiledShader != nullptr );
		}

		virtual ~GlobalShaderCommon() = default;
		GlobalShaderCommon( const GlobalShaderCommon& ) = default;
		GlobalShaderCommon( GlobalShaderCommon&& ) = default;
		GlobalShaderCommon& operator=( const GlobalShaderCommon& ) = default;
		GlobalShaderCommon& operator=( GlobalShaderCommon&& ) = default;

		operator ShaderType* () const
		{
			return GetShader();
		}

	protected:
		ShaderType* GetShader() const
		{
			return  Cast<ShaderType>( m_compiledShader );
		}

		IShader* m_shader = nullptr;
		ShaderType* m_compiledShader = nullptr;
	};
}

#define REGISTER_GLOBAL_SHADER( type, shaderPath ) \
static_assert( std::is_base_of_v<GlobalShaderBase, type>, "GlobalShader must inherit GlobalShaderBase" ); \
GlobalShaderRegister type##_register( typeid( type ), shaderPath );
