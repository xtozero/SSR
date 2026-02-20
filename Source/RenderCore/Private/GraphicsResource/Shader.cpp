#include "Shader.h"

#include "ArchiveUtility.h"
#include "TaskScheduler.h"

#include <mutex>

namespace fs = std::filesystem;

namespace
{
	using namespace ::rendercore;

	std::mutex ShaderAssetsMutex;
	std::vector<ShaderAsset*> ShaderAssets;

	void RegisterShaderAsset(ShaderAsset* shader)
	{
		std::lock_guard<std::mutex> lock( ShaderAssetsMutex );
		ShaderAssets.emplace_back( shader );
	}

	void UnregisterShaderAsset( ShaderAsset* shader )
	{
		std::lock_guard<std::mutex> lock( ShaderAssetsMutex );
		std::erase_if( ShaderAssets,
					   [shader]( const ShaderAsset* shaderAsset )
					   {
						   return shaderAsset == shader;
					   } );
	}
}

namespace rendercore
{
	void ShaderBase::SetHash( size_t hash )
	{
		m_hash = hash;

		if ( IsValid() )
		{
			m_shader->SetHash( hash );
		}
	}

	size_t ShaderBase::GetHash() const
	{
		return m_hash;
	}

	StaticShaderSwitches ShaderAsset::GetStaticSwitches() const
	{
		return m_switches;
	}

	ShaderAsset::ShaderAsset( const StaticShaderSwitches& switches )
		: m_switches( switches )
	{
		RegisterShaderAsset( this );
	}

	ShaderAsset::ShaderAsset()
	{
		RegisterShaderAsset( this );
	}

	ShaderAsset::~ShaderAsset()
	{
		UnregisterShaderAsset( this );
	}

	ShaderBase* ShaderBase::CompileShader( [[maybe_unused]] const StaticShaderSwitches& switches )
	{
		return this;
	}

	void ShaderBase::RecompileShader()
	{
		// Shader not currently used in scene rendering. Modify to allow redirection even if unused in the future.
		if (m_parent.get() == nullptr)
		{
			return;
		}

		auto uberShader = static_cast<UberShader*>( m_parent.get() );

		BinaryChunk byteCode = uberShader->ComipeShaderByteCode( GetStaticSwitches() );
		if (byteCode.Size() == 0)
		{
			return;
		}

		m_byteCode = std::move( byteCode );

		m_parameterMap.Clear();
		m_parameterInfo.Clear();
		GraphicsInterface().BuildShaderMetaData( ByteCode(), ParameterMap(), ParameterInfo() );
		RecreateShader();
	}

	void ShaderBase::ReloadShaders()
	{
		std::vector<ShaderBase*> shaders;
		std::set<UberShader*> shadersToReload;

		for ( ShaderAsset* shaderAsset : ShaderAssets )
		{
			if ( auto uberShader = Cast<UberShader>( shaderAsset ) )
			{
				fs::file_time_type curLastWriteTime = fs::last_write_time( uberShader->Path() );
				if ( curLastWriteTime == uberShader->LastWriteTime() )
				{
					continue;
				}

				shadersToReload.emplace( uberShader );
			}
			else
			{
				shaders.emplace_back( Cast<ShaderBase>( shaderAsset ) );
			}
		}

		std::atomic<int32> numReloadedShaders = static_cast<int32>( shadersToReload.size() );

		TaskHandle handle = EnqueueThreadTask<ThreadType::RenderThread>(
			[]()
			{
				GetInterface<agl::IAgl>()->WaitGPU();
			});
		GetInterface<ITaskScheduler>()->Wait( handle );

		for ( UberShader* uberShader : shadersToReload )
		{
			IAssetLoader::LoadCompletionCallback onLoadComplete;
			onLoadComplete.BindFunctor(
				[uberShader, &numReloadedShaders]( const std::shared_ptr<void>& asset )
				{
					auto newUberShader = std::reinterpret_pointer_cast<UberShader>( asset );
					*uberShader = *newUberShader.get();

					--numReloadedShaders;
				} );

			GetInterface<IAssetLoader>()->RequestAsyncLoad( uberShader->Path().generic_string(), onLoadComplete, false );
		}

		while ( numReloadedShaders > 0 )
		{
			GetInterface<ITaskScheduler>()->ProcessThisThreadTask();
		}

		for ( ShaderBase* shader : shaders )
		{
			auto parentShader = Cast<UberShader>( shader->GetParent() );
			if ( shadersToReload.contains( parentShader ) == false )
			{
				continue;
			}

			shader->RecompileShader();
		}
	}

	ShaderAsset* ShaderBase::GetParent()
	{
		return const_cast<ShaderAsset*>( static_cast<const ShaderBase&>( *this ).GetParent() );
	}

	const ShaderAsset* ShaderBase::GetParent() const
	{
		return m_parent.get();
	}

	void ShaderBase::SetParent( const std::shared_ptr<ShaderAsset>& parent )
	{
		assert( m_parent == nullptr || m_parent == parent );
		m_parent = parent;
	}

	void ShaderBase::PostLoadImpl()
	{
		CreateShader();
	}

	void ShaderBase::RecreateShader()
	{
		EnqueueRenderTask(
			[this, shader = m_shader]()
			{
				shader->UpdateByteCodeAndParameterInfo( m_byteCode.Data(), m_byteCode.Size(), m_parameterInfo );
				shader->Free();
				shader->Init();
			} );
	}

	REGISTER_ASSET( VertexShader );

	agl::VertexShader* VertexShader::Resource()
	{
		return static_cast<agl::VertexShader*>( m_shader.Get() );
	}

	const agl::VertexShader* VertexShader::Resource() const
	{
		return static_cast<agl::VertexShader*>( m_shader.Get() );
	}

	void VertexShader::CreateShader()
	{
		m_shader = agl::VertexShader::Create( m_byteCode.Data(), m_byteCode.Size(), m_parameterInfo ).Get();
		m_shader->SetHash( m_hash );

		EnqueueRenderTask(
			[shader = m_shader]()
			{
				shader->Init();
			} );
	}

	REGISTER_ASSET( GeometryShader );

	agl::GeometryShader* GeometryShader::Resource()
	{
		return static_cast<agl::GeometryShader*>( m_shader.Get() );
	}

	const agl::GeometryShader* GeometryShader::Resource() const
	{
		return  static_cast<agl::GeometryShader*>( m_shader.Get() );
	}

	void GeometryShader::CreateShader()
	{
		m_shader = agl::GeometryShader::Create( m_byteCode.Data(), m_byteCode.Size(), m_parameterInfo ).Get();
		m_shader->SetHash( m_hash );

		EnqueueRenderTask(
			[shader = m_shader]()
			{
				shader->Init();
			} );
	}


	REGISTER_ASSET( PixelShader );

	agl::PixelShader* PixelShader::Resource()
	{
		return static_cast<agl::PixelShader*>( m_shader.Get() );
	}

	const agl::PixelShader* PixelShader::Resource() const
	{
		return static_cast<agl::PixelShader*>( m_shader.Get() );
	}

	void PixelShader::CreateShader()
	{
		m_shader = agl::PixelShader::Create( m_byteCode.Data(), m_byteCode.Size(), m_parameterInfo ).Get();
		m_shader->SetHash( m_hash );

		EnqueueRenderTask(
			[shader = m_shader]()
			{
				shader->Init();
			} );
	}

	REGISTER_ASSET( ComputeShader );

	agl::ComputeShader* ComputeShader::Resource()
	{
		return static_cast<agl::ComputeShader*>( m_shader.Get() );
	}

	const agl::ComputeShader* ComputeShader::Resource() const
	{
		return static_cast<agl::ComputeShader*>( m_shader.Get() );
	}

	void ComputeShader::CreateShader()
	{
		m_shader = agl::ComputeShader::Create( m_byteCode.Data(), m_byteCode.Size(), m_parameterInfo ).Get();
		m_shader->SetHash( m_hash );

		EnqueueRenderTask(
			[shader = m_shader]()
			{
				shader->Init();
			} );
	}

	REGISTER_ASSET( MeshShader );
	ShaderBase* MeshShader::CompileShader( const StaticShaderSwitches& switches )
	{
		if ( GetInterface<agl::IAgl>()->SupportsMeshShader() == false )
		{
			return nullptr;
		}

		return Super::CompileShader( switches );
	}

	agl::MeshShader* MeshShader::Resource()
	{
		return static_cast<agl::MeshShader*>( m_shader.Get() );
	}

	const agl::MeshShader* MeshShader::Resource() const
	{
		return static_cast<agl::MeshShader*>( m_shader.Get() );
	}

	void MeshShader::CreateShader()
	{
		m_shader = agl::MeshShader::Create( m_byteCode.Data(), m_byteCode.Size(), m_parameterInfo ).Get();
		m_shader->SetHash( m_hash );

		EnqueueRenderTask(
			[shader = m_shader]()
			{
				shader->Init();
			} );
	}

	REGISTER_ASSET( AmplificationShader );
	ShaderBase* AmplificationShader::CompileShader( const StaticShaderSwitches& switches )
	{
		if ( GetInterface<agl::IAgl>()->SupportsMeshShader() == false )
		{
			return nullptr;
		}

		return Super::CompileShader( switches );
	}

	agl::AmplificationShader* AmplificationShader::Resource()
	{
		return static_cast<agl::AmplificationShader*>( m_shader.Get() );
	}

	const agl::AmplificationShader* AmplificationShader::Resource() const
	{
		return static_cast<agl::AmplificationShader*>( m_shader.Get() );
	}

	void AmplificationShader::CreateShader()
	{
		m_shader = agl::AmplificationShader::Create( m_byteCode.Data(), m_byteCode.Size(), m_parameterInfo ).Get();
		m_shader->SetHash( m_hash );

		EnqueueRenderTask(
			[shader = m_shader]()
			{
				shader->Init();
			} );
	}

	REGISTER_ASSET( RayGenerationShader );
	ShaderBase* RayGenerationShader::CompileShader( const StaticShaderSwitches& switches )
	{
		if ( GetInterface<agl::IAgl>()->SupportsHardwareRayTracing() == false )
		{
			return nullptr;
		}

		return ShaderBase::CompileShader( switches );
	}

	agl::RayGenerationShader* RayGenerationShader::Resource()
	{
		return static_cast<agl::RayGenerationShader*>( m_shader.Get() );
	}

	const agl::RayGenerationShader* RayGenerationShader::Resource() const
	{
		return static_cast<agl::RayGenerationShader*>( m_shader.Get() );
	}

	void RayGenerationShader::CreateShader()
	{
		m_shader = agl::RayGenerationShader::Create( m_byteCode.Data(), m_byteCode.Size(), m_parameterInfo ).Get();
		m_shader->SetHash( m_hash );

		EnqueueRenderTask(
			[shader = m_shader]()
			{
				shader->Init();
			} );
	}

	REGISTER_ASSET( IntersectionShader );
	ShaderBase* IntersectionShader::CompileShader( const StaticShaderSwitches& switches )
	{
		if ( GetInterface<agl::IAgl>()->SupportsHardwareRayTracing() == false )
		{
			return nullptr;
		}

		return ShaderBase::CompileShader( switches );
	}

	agl::IntersectionShader* IntersectionShader::Resource()
	{
		return static_cast<agl::IntersectionShader*>( m_shader.Get() );
	}

	const agl::IntersectionShader* IntersectionShader::Resource() const
	{
		return static_cast<agl::IntersectionShader*>( m_shader.Get() );
	}

	void IntersectionShader::CreateShader()
	{
		m_shader = agl::IntersectionShader::Create( m_byteCode.Data(), m_byteCode.Size(), m_parameterInfo ).Get();
		m_shader->SetHash( m_hash );

		EnqueueRenderTask(
			[shader = m_shader]()
			{
				shader->Init();
			} );
	}

	REGISTER_ASSET( AnyHitShader );
	ShaderBase* AnyHitShader::CompileShader( const StaticShaderSwitches& switches )
	{
		if ( GetInterface<agl::IAgl>()->SupportsHardwareRayTracing() == false )
		{
			return nullptr;
		}

		return ShaderBase::CompileShader( switches );
	}

	agl::AnyHitShader* AnyHitShader::Resource()
	{
		return static_cast<agl::AnyHitShader*>( m_shader.Get() );
	}

	const agl::AnyHitShader* AnyHitShader::Resource() const
	{
		return static_cast<agl::AnyHitShader*>( m_shader.Get() );
	}

	void AnyHitShader::CreateShader()
	{
		m_shader = agl::AnyHitShader::Create( m_byteCode.Data(), m_byteCode.Size(), m_parameterInfo ).Get();
		m_shader->SetHash( m_hash );

		EnqueueRenderTask(
			[shader = m_shader]()
			{
				shader->Init();
			} );
	}

	REGISTER_ASSET( ClosestHitShader );
	ShaderBase* ClosestHitShader::CompileShader( const StaticShaderSwitches& switches )
	{
		if ( GetInterface<agl::IAgl>()->SupportsHardwareRayTracing() == false )
		{
			return nullptr;
		}

		return ShaderBase::CompileShader( switches );
	}

	agl::ClosestHitShader* ClosestHitShader::Resource()
	{
		return static_cast<agl::ClosestHitShader*>( m_shader.Get() );
	}

	const agl::ClosestHitShader* ClosestHitShader::Resource() const
	{
		return static_cast<agl::ClosestHitShader*>( m_shader.Get() );
	}

	void ClosestHitShader::CreateShader()
	{
		m_shader = agl::ClosestHitShader::Create( m_byteCode.Data(), m_byteCode.Size(), m_parameterInfo ).Get();
		m_shader->SetHash( m_hash );

		EnqueueRenderTask(
			[shader = m_shader]()
			{
				shader->Init();
			} );
	}

	REGISTER_ASSET( MissShader );
	ShaderBase* MissShader::CompileShader( const StaticShaderSwitches& switches )
	{
		if ( GetInterface<agl::IAgl>()->SupportsHardwareRayTracing() == false )
		{
			return nullptr;
		}

		return ShaderBase::CompileShader( switches );
	}

	agl::MissShader* MissShader::Resource()
	{
		return static_cast<agl::MissShader*>( m_shader.Get() );
	}

	const agl::MissShader* MissShader::Resource() const
	{
		return static_cast<agl::MissShader*>( m_shader.Get() );
	}

	void MissShader::CreateShader()
	{
		m_shader = agl::MissShader::Create( m_byteCode.Data(), m_byteCode.Size(), m_parameterInfo ).Get();
		m_shader->SetHash( m_hash );

		EnqueueRenderTask(
			[shader = m_shader]()
			{
				shader->Init();
			} );
	}

	REGISTER_ASSET( CallableShader );
	ShaderBase* CallableShader::CompileShader( const StaticShaderSwitches& switches )
	{
		if ( GetInterface<agl::IAgl>()->SupportsHardwareRayTracing() == false )
		{
			return nullptr;
		}

		return ShaderBase::CompileShader( switches );
	}

	agl::CallableShader* CallableShader::Resource()
	{
		return static_cast<agl::CallableShader*>( m_shader.Get() );
	}

	const agl::CallableShader* CallableShader::Resource() const
	{
		return static_cast<agl::CallableShader*>( m_shader.Get() );
	}

	void CallableShader::CreateShader()
	{
		m_shader = agl::CallableShader::Create( m_byteCode.Data(), m_byteCode.Size(), m_parameterInfo ).Get();
		m_shader->SetHash( m_hash );

		EnqueueRenderTask(
			[shader = m_shader]()
			{
				shader->Init();
			} );
	}
}
