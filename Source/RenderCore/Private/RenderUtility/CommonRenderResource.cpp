#include "CommonRenderResource.h"

#include "ColorTypes.h"
#include "CommandList.h"
#include "ComputePipelineState.h"
#include "GlobalShaders.h"
#include "IAssetLoader.h"

namespace
{
	agl::RefHandle<agl::Texture> CreateTexture( Color color, const char* debugName )
	{
		agl::TextureTrait trait = {
				.m_width = 1,
				.m_height = 1,
				.m_depth = 1,
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
				.m_access = agl::ResourceAccessFlag::Default,
				.m_bindType = agl::ResourceBindType::ShaderResource,
				.m_miscFlag = agl::ResourceMisc::None
		};

		agl::ResourceInitData initData = {
			.m_srcData = &color.DWORD(),
			.m_srcSize = sizeof( color.DWORD() )
		};

		initData.m_sections.emplace_back();
		agl::ResourceSectionData& section = initData.m_sections.back();
		section.m_offset = 0;
		section.m_pitch = section.m_slicePitch = sizeof( uint32 );

		agl::RefHandle<agl::Texture> texture = agl::Texture::Create( trait, debugName, &initData );
		EnqueueRenderTask(
			[texture]()
			{
				texture->Init();
			} );

		return texture;
	}

	agl::RefHandle<agl::Texture> CreateCubeTexture( Color color, const char* debugName )
	{
		agl::TextureTrait trait = {
				.m_width = 1,
				.m_height = 1,
				.m_depth = 6,
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
				.m_access = agl::ResourceAccessFlag::Default,
				.m_bindType = agl::ResourceBindType::ShaderResource,
				.m_miscFlag = agl::ResourceMisc::TextureCube
		};

		uint32 clearColor[6] = { color.DWORD(), color.DWORD(), color.DWORD(), color.DWORD(), color.DWORD(), color.DWORD() };
		agl::ResourceInitData initData = {
			.m_srcData = clearColor,
			.m_srcSize = sizeof( clearColor )
		};

		for ( int32 i = 0; i < 6; ++i )
		{
			initData.m_sections.emplace_back();
			agl::ResourceSectionData& section = initData.m_sections.back();

			section.m_offset = i * sizeof( uint32 );
			section.m_pitch = section.m_slicePitch = sizeof( uint32 );
		}

		agl::RefHandle<agl::Texture> cubeTexture = agl::Texture::Create( trait, debugName, &initData );
		EnqueueRenderTask(
			[cubeTexture]()
			{
				cubeTexture->Init();
			} );

		return cubeTexture;
	}
}

namespace rendercore
{
	class PrecomputedBrdfCS final : public GlobalShaderCommon<ComputeShader, PrecomputedBrdfCS>
	{
	private:
		DEFINE_SHADER_PARAM( Precomputed );
	};

	agl::RefHandle<agl::Texture> CreateBRDFLookUpTexture()
	{
		agl::TextureTrait trait = {
			.m_width = 512,
			.m_height = 512,
			.m_depth = 1,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R16G16_FLOAT,
			.m_access = agl::ResourceAccessFlag::Default,
			.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess,
			.m_miscFlag = agl::ResourceMisc::None
		};

		agl::RefHandle<agl::Texture> brdfLUT = agl::Texture::Create( trait, "BrdfLookUpTexture", nullptr );
		EnqueueRenderTask(
			[brdfLUT]()
			{
				brdfLUT->Init();

				PrecomputedBrdfCS precomputedBrdfCS;
				agl::RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( precomputedBrdfCS );

				auto commandList = GetCommandList();
				commandList.BindPipelineState( pso );

				agl::ShaderBindings shaderBindings = CreateShaderBindings( precomputedBrdfCS );
				BindResource( shaderBindings, precomputedBrdfCS.Precomputed(), brdfLUT );

				commandList.BindShaderResources( shaderBindings );

				commandList.Dispatch( 512 / 8, 512 / 8 );

				commandList.AddTransition( Transition( *brdfLUT.Get(), agl::ResourceState::PixelShaderResource ) );

				commandList.Commit();
			} );

		return brdfLUT;
	}

	REGISTER_GLOBAL_SHADER( FullScreenQuadVS, "./Assets/Shaders/Common/VS_FullScreenQuad.asset" );
	REGISTER_GLOBAL_SHADER( PrecomputedBrdfCS, "./Assets/Shaders/PhysicallyBased/CS_PrecomputedBRDF.asset" );

	void DefaultGraphicsResources::BootUp()
	{
		BlackTexture = CreateTexture( Color::Black, "DefaultBlack" );
		WhiteTexture = CreateTexture( Color::White, "DefaultWhite" );

		BlackCubeTexture = CreateCubeTexture( Color::Black, "DefaultBlackCube" );
		WhiteCubeTexture = CreateCubeTexture( Color::White, "DefaultWhiteCube" );
		
		{
			std::filesystem::path assetPath = "./Assets/EngineDefault/Texture/PrecomputedBRDF.asset";
			if ( std::filesystem::exists( assetPath ) )
			{
				IAssetLoader::LoadCompletionCallback onLoadComplete;
				onLoadComplete.BindFunctor(
					[this]( const std::shared_ptr<void>& asset )
					{
						auto brdfLUT = std::reinterpret_pointer_cast<DDSTexture>( asset );
						BRDFLookUpTexture = brdfLUT->Resource();
						--m_numPending;
					} );

				AssetLoaderSharedHandle handle = GetInterface<IAssetLoader>()->RequestAsyncLoad( "./Assets/EngineDefault/Texture/PrecomputedBRDF.asset", onLoadComplete );

				assert( handle->IsLoadingInProgress() || handle->IsLoadComplete() );
				++m_numPending;
			}
			else
			{
				BRDFLookUpTexture = CreateBRDFLookUpTexture();
			}
		}
	}

	void DefaultGraphicsResources::Shutdown()
	{
		EnqueueRenderTask(
			[]()
			{
				BlackTexture = nullptr;
				WhiteTexture = nullptr;

				BlackCubeTexture = nullptr;
				WhiteCubeTexture = nullptr;

				BRDFLookUpTexture = nullptr;
			} );
	}

	bool DefaultGraphicsResources::IsReady() const
	{
		return m_numPending == 0;
	}

	agl::RefHandle<agl::Texture> BlackTexture;
	agl::RefHandle<agl::Texture> WhiteTexture;

	agl::RefHandle<agl::Texture> BlackCubeTexture;
	agl::RefHandle<agl::Texture> WhiteCubeTexture;

	agl::RefHandle<agl::Texture> BRDFLookUpTexture;
}
