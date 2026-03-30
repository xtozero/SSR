#include "CommonRenderResource.h"

#include "ColorTypes.h"
#include "CommandList.h"
#include "ComputePipelineState.h"
#include "GlobalShaders.h"
#include "IAssetLoader.h"
#include "Platform/CommandLine.h"
#include "RenderGraph.h"

using ::rendercore::IndexBuffer;

namespace
{
	RefHandle<agl::Texture> CreateTexture( Color color, const char* debugName )
	{
		agl::TextureDesc desc = {
				.m_width = 1,
				.m_height = 1,
				.m_depth = 1,
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
				.m_access = agl::ResourceAccess::Default,
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

		RefHandle<agl::Texture> texture = agl::Texture::Create( desc, debugName, &initData );
		return texture;
	}

	RefHandle<agl::Texture> CreateCubeTexture( Color color, const char* debugName )
	{
		agl::TextureDesc desc = {
				.m_width = 1,
				.m_height = 1,
				.m_depth = 6,
				.m_sampleCount = 1,
				.m_sampleQuality = 0,
				.m_mipLevels = 1,
				.m_format = agl::ResourceFormat::R8G8B8A8_UNORM,
				.m_access = agl::ResourceAccess::Default,
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

		RefHandle<agl::Texture> cubeTexture = agl::Texture::Create( desc, debugName, &initData );
		return cubeTexture;
	}

	IndexBuffer CreateOcclusionQueryIndexBuffer()
	{
		uint16 cubeIndex[36] = {
			0, 1, 2,
			2, 1, 3,
			4, 5, 0,
			0, 5, 1,
			2, 3, 6,
			6, 3, 7,
			6, 7, 4,
			4, 7, 5,
			1, 5, 3,
			3, 5, 7,
			4, 0, 6,
			6, 0, 2
		};
		return IndexBuffer( 36, agl::ResourceState::Common, cubeIndex, false );
	}
}

namespace rendercore
{
	class PrecomputedBrdfCS final : public GlobalShaderBase<ComputeShader, PrecomputedBrdfCS>
	{
	private:
		DEFINE_SHADER_PARAM( Precomputed );
	};

	class DenoiseCS final : public GlobalShaderBase<ComputeShader, DenoiseCS>
	{
		DEFINE_SHADER_PARAM( PrevImage );
		DEFINE_SHADER_PARAM( Image );
		DEFINE_SHADER_PARAM( PrevViewSpaceDistance );
		DEFINE_SHADER_PARAM( ViewSpaceDistance );
		DEFINE_SHADER_PARAM( Velocity );

		DEFINE_SHADER_PARAM( Sampler );

		DEFINE_SHADER_PARAM( Denoised );

		DEFINE_SHADER_PARAM( KernelRadius );
		DEFINE_SHADER_PARAM( ScreenSize );
		DEFINE_SHADER_PARAM( InvScreenSize );
	};

	REGISTER_GLOBAL_SHADER( FullScreenQuadVS, "Common/VS_FullScreenQuad.fx", "main" );
	REGISTER_GLOBAL_SHADER( PrecomputedBrdfCS, "PhysicallyBased/CS_PrecomputedBRDF.fx", "main" );
	REGISTER_GLOBAL_SHADER( DefaultAS, "Material/AS_Meshlet.fx", "main" );
	REGISTER_GLOBAL_SHADER( DenoiseCS, "Common/CS_Denoise.fx", "main" );

	RefHandle<agl::Texture> CreateBRDFLookUpTexture()
	{
		agl::TextureDesc desc = {
			.m_width = 512,
			.m_height = 512,
			.m_depth = 1,
			.m_sampleCount = 1,
			.m_sampleQuality = 0,
			.m_mipLevels = 1,
			.m_format = agl::ResourceFormat::R16G16_FLOAT,
			.m_access = agl::ResourceAccess::Default,
			.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess,
			.m_miscFlag = agl::ResourceMisc::None
		};

		RefHandle<agl::Texture> brdfLUT = agl::Texture::Create( desc, "BrdfLookUpTexture", nullptr );
		EnqueueRenderTask(
			[brdfLUT]()
			{
				PrecomputedBrdfCS precomputedBrdfCS;
				RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( precomputedBrdfCS );

				auto commandList = GetCommandList();
				commandList.BindPipelineState( pso.Get() );

				agl::ShaderBindings shaderBindings = CreateShaderBindings( precomputedBrdfCS );
				BindResource( shaderBindings, precomputedBrdfCS.Precomputed(), brdfLUT.Get() );

				commandList.BindShaderResources( shaderBindings );

				commandList.Dispatch( 512 / 8, 512 / 8 );

				commandList.Commit();
			} );

		return brdfLUT;
	}

	bool DenoisePassParams::IsValid() const
	{
		return ( m_prevImage != nullptr )
				&& HasAnyFlags( m_prevImage->GetDesc().m_bindType, agl::ResourceBindType::ShaderResource )
				&& ( m_image != nullptr )
				&& HasAnyFlags( m_image->GetDesc().m_bindType, agl::ResourceBindType::RandomAccess )
				&& ( m_prevViewSpaceDistance != nullptr )
				&& ( m_viewSpaceDistance != nullptr )
				&& ( m_velocity != nullptr )
				&& ( m_kernelRadius > 0 )
				&& ( m_screenSize.x > 0 )
				&& ( m_screenSize.y > 0 );
	}

	RefHandle<agl::Texture> AddDenoisePass( RenderGraph& renderGraph, const DenoisePassParams& params )
	{
		assert( params.IsValid() );

		auto denoisedDesc = params.m_image->GetDesc();
		denoisedDesc.m_bindType |= agl::ResourceBindType::RandomAccess;

		auto rgDenoised = renderGraph.CreateTexture( denoisedDesc, "Denoised" );

		BEGIN_RG_RESOURCE_STRUCT( DenoisePassResource )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( prevImage )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( image )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( prevViewSpaceDistance )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( viewSpaceDistance )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( velocity )
			DECLARE_RG_TEXTURE_UAV( denoised )
		END_RG_RESOURCE_STRUCT();

		DenoisePassResource passResource = {
			.m_prevImage = params.m_prevImage,
			.m_image = params.m_image,
			.m_prevViewSpaceDistance = params.m_prevViewSpaceDistance,
			.m_viewSpaceDistance = params.m_viewSpaceDistance,
			.m_velocity = params.m_velocity,
			.m_denoised = rgDenoised,
		};

		struct PassParams
		{
			int32 m_kernelRadius;
			Vector2 m_screenSize;
		} passParams = {
			.m_kernelRadius = params.m_kernelRadius,
			.m_screenSize = params.m_screenSize,
		};

		renderGraph.AddPass( passResource,
			[passResource, passParams]( ComputeCommandList& commandList )
			{
				DenoiseCS denoiseCS;

				RefHandle<agl::ComputePipelineState> denoisePSO = PrepareComputePipelineState( denoiseCS );
				commandList.BindPipelineState( denoisePSO.Get() );

				agl::ShaderBindings shaderBindings = CreateShaderBindings( denoiseCS );

				BindResource( shaderBindings, denoiseCS.PrevImage(), passResource.m_prevImage->Get() );
				BindResource( shaderBindings, denoiseCS.Image(), passResource.m_image->Get() );
				BindResource( shaderBindings, denoiseCS.PrevViewSpaceDistance(), passResource.m_prevViewSpaceDistance->Get() );
				BindResource( shaderBindings, denoiseCS.ViewSpaceDistance(), passResource.m_viewSpaceDistance->Get() );
				BindResource( shaderBindings, denoiseCS.Velocity(), passResource.m_velocity->Get() );
				BindResource( shaderBindings, denoiseCS.Denoised(), passResource.m_denoised->Get() );

				SamplerState blackBorderSampler = StaticSamplerState<agl::TextureFilter::MinMagMipLinear
					, agl::TextureAddressMode::Border
					, agl::TextureAddressMode::Border
					, agl::TextureAddressMode::Border
					, 0.f
					, agl::ComparisonFunc::Never
					, Color( 0, 0, 0, 255 )>::Get();
				BindResource( shaderBindings, denoiseCS.Sampler(), blackBorderSampler );

				SetShaderValue( commandList, denoiseCS.KernelRadius(), passParams.m_kernelRadius );
				SetShaderValue( commandList, denoiseCS.ScreenSize(), passParams.m_screenSize );
				SetShaderValue( commandList, denoiseCS.InvScreenSize(), Vector2::OneVector / passParams.m_screenSize );

				commandList.BindShaderResources( shaderBindings );

				auto numThreadGroupX = static_cast<uint32>( std::ceilf( passParams.m_screenSize.x / 8 ) );
				auto numThreadGroupY = static_cast<uint32>( std::ceilf( passParams.m_screenSize.y / 8 ) );
				commandList.Dispatch( numThreadGroupX, numThreadGroupY, 1 );
			} );

		return renderGraph.ConvertToExternalResource( rgDenoised );
	}

	void DefaultGraphicsResources::BootUp()
	{
		BlackTexture = CreateTexture( Color::Black, "DefaultBlack" );
		WhiteTexture = CreateTexture( Color::White, "DefaultWhite" );

		BlackCubeTexture = CreateCubeTexture( Color::Black, "DefaultBlackCube" );
		WhiteCubeTexture = CreateCubeTexture( Color::White, "DefaultWhiteCube" );

		OcclusionQueryIndexBuffer = CreateOcclusionQueryIndexBuffer();
		
		bool isRunningAssetBuilder = engine::CommandLine::Has( StaticName( "AssetBuilder" ) );
		if ( isRunningAssetBuilder )
		{
			return;
		}

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

			AssetLoaderSharedHandle handle = GetInterface<IAssetLoader>()->RequestAsyncLoad( assetPath.generic_string(), onLoadComplete );

			assert( handle->IsLoadingInProgress() || handle->IsLoadComplete() );
			++m_numPending;
		}
		else
		{
			BRDFLookUpTexture = CreateBRDFLookUpTexture();
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

				std::destroy_at( &OcclusionQueryIndexBuffer );

				BRDFLookUpTexture = nullptr;
			} );
	}

	bool DefaultGraphicsResources::IsReady() const
	{
		return m_numPending == 0;
	}

	RefHandle<agl::Texture> BlackTexture;
	RefHandle<agl::Texture> WhiteTexture;

	RefHandle<agl::Texture> BlackCubeTexture;
	RefHandle<agl::Texture> WhiteCubeTexture;

	RefHandle<agl::Texture> BRDFLookUpTexture;

	IndexBuffer OcclusionQueryIndexBuffer;
}
