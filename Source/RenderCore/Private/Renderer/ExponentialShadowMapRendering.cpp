#include "ExponentialShadowMapRendering.h"

#include "CommandList.h"
#include "ComputePipelineState.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "GlobalShaders.h"
#include "InlineMemoryAllocator.h"
#include "Multithread/TaskScheduler.h"
#include "Proxies/LightProxy.h"
#include "Scene/LightSceneInfo.h"
#include "Scene/ShadowInfo.h"
#include "ShaderParameterUtils.h"

#include <cassert>
#include <numbers>

namespace
{
	float Gaussian( int32 x, float sigma )
	{
		constexpr float pi = std::numbers::pi_v<float>;
		return 1.f / ( std::sqrtf( 2.f * pi ) * sigma ) * std::expf( -( x * x ) / ( 2.f * sigma * sigma ) );
	}
}

namespace rendercore
{
	class CascadedESMsBlurCS : public GlobalShaderCommon<ComputeShader, CascadedESMsBlurCS>
	{
		using GlobalShaderCommon::GlobalShaderCommon;

	public:
		CascadedESMsBlurCS() = default;

	private:
		DEFINE_SHADER_PARAM( KernelBuffer );
		DEFINE_SHADER_PARAM( SrcTexture );
		DEFINE_SHADER_PARAM( PointSampler );
		DEFINE_SHADER_PARAM( DestTexture );
	};

	class CascadedESMsCS : public GlobalShaderCommon<ComputeShader, CascadedESMsCS>
	{
	private:
		DEFINE_SHADER_PARAM( SrcTexture );
		DEFINE_SHADER_PARAM( ESMsTexture );
		DEFINE_SHADER_PARAM( ParameterC );
	};

	REGISTER_GLOBAL_SHADER( CascadedESMsBlurCS, "./Assets/Shaders/Shadow/CS_CascadedESMsBlur.asset" );
	REGISTER_GLOBAL_SHADER( CascadedESMsCS, "./Assets/Shaders/Shadow/CS_CascadedESMs.asset" );

	template <float Sigma, int32 KernelSize>
	agl::RefHandle<agl::Texture> ApplyGaussianBlur( agl::RefHandle<agl::Texture> srcTexture )
	{
		static_assert( KernelSize < 128, "Maximum kernel size exceeded" );

		float kernel[128] = {};
		for ( int32 x = -KernelSize / 2, i = 0; x <= KernelSize / 2; ++x )
		{
			kernel[i++] = Gaussian( x, Sigma );
		}

		const agl::TextureTrait& srcTrait = srcTexture->GetTrait();
		agl::TextureTrait tempTrait = srcTrait;
		tempTrait.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess;

		agl::RefHandle<agl::Texture> tempTexture = agl::Texture::Create( tempTrait );
		assert( tempTexture );

		tempTexture->Init();

		StaticShaderSwitches horizonSwitches = CascadedESMsBlurCS::GetSwitches();
		horizonSwitches.On( Name( "KernelSize" ), KernelSize );

		CascadedESMsBlurCS horizonBlurCS( horizonSwitches );
		agl::RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( horizonBlurCS );

		auto commandList = GetCommandList();
		commandList.BindPipelineState( pso );

		SamplerOption pointSamperOption;
		pointSamperOption.m_filter = agl::TextureFilter::Point;
		auto pointSampler = GraphicsInterface().FindOrCreate( pointSamperOption );

		agl::ShaderBindings shaderBindings = CreateShaderBindings( horizonBlurCS );
		BindResource( shaderBindings, horizonBlurCS.SrcTexture(), srcTexture );
		BindResource( shaderBindings, horizonBlurCS.PointSampler(), pointSampler );
		BindResource( shaderBindings, horizonBlurCS.DestTexture(), tempTexture );

		SetShaderValue( commandList, horizonBlurCS.KernelBuffer(), kernel );

		commandList.BindShaderResources( shaderBindings );

		uint32 w = srcTrait.m_width;
		uint32 h = srcTrait.m_height;

		assert( ( w % 8 == 0 ) && ( h % 8 == 0 ) );

		const uint32 threadGroupCount[3] = {
			static_cast<uint32>( std::ceilf( w / 8.f ) ),
			static_cast<uint32>( std::ceilf( h / 8.f ) ),
			srcTrait.m_depth
		};

		commandList.Dispatch( threadGroupCount[0], threadGroupCount[1], threadGroupCount[2] );

		StaticShaderSwitches virticalSwitches = horizonSwitches;
		virticalSwitches.On( Name( "Virtical" ), 1 );
		virticalSwitches.On( Name( "KernelSize" ), KernelSize );

		CascadedESMsBlurCS virticalBlurCS( virticalSwitches );
		pso = PrepareComputePipelineState( virticalBlurCS );
		commandList.BindPipelineState( pso );

		shaderBindings = CreateShaderBindings( virticalBlurCS );
		BindResource( shaderBindings, virticalBlurCS.SrcTexture(), tempTexture );
		BindResource( shaderBindings, virticalBlurCS.PointSampler(), pointSampler );
		BindResource( shaderBindings, virticalBlurCS.DestTexture(), srcTexture );

		SetShaderValue( commandList, virticalBlurCS.KernelBuffer(), kernel );

		commandList.BindShaderResources( shaderBindings );

		commandList.Dispatch( threadGroupCount[0], threadGroupCount[1], threadGroupCount[2] );

		return srcTexture;
	}

	agl::RefHandle<agl::Texture> GenerateCascadedESMs( agl::RefHandle<agl::Texture> shadowMap )
	{
		assert( IsInRenderThread() );

		const agl::TextureTrait& srcTrait = shadowMap->GetTrait();
		agl::TextureTrait esmsTrait = srcTrait;
		esmsTrait.m_format = agl::ResourceFormat::R32_FLOAT;
		esmsTrait.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess;

		agl::RefHandle<agl::Texture> esmsTexture = agl::Texture::Create( esmsTrait );
		assert( esmsTexture );

		esmsTexture->Init();

		CascadedESMsCS cascadedESMsCS;

		agl::RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( cascadedESMsCS );

		auto commandList = GetCommandList();
		commandList.BindPipelineState( pso );

		agl::ShaderBindings shaderBindings = CreateShaderBindings( cascadedESMsCS );
		BindResource( shaderBindings, cascadedESMsCS.SrcTexture(), shadowMap );
		BindResource( shaderBindings, cascadedESMsCS.ESMsTexture(), esmsTexture );

		SetShaderValue( commandList, cascadedESMsCS.ParameterC(), DefaultRenderCore::ESMsParamC() );

		commandList.BindShaderResources( shaderBindings );

		uint32 w = srcTrait.m_width;
		uint32 h = srcTrait.m_height;

		assert( ( w % 8 == 0 ) && ( h % 8 == 0 ) );

		const uint32 threadGroupCount[3] = {
			static_cast<uint32>( std::ceilf( w / 8.f ) ),
			static_cast<uint32>( std::ceilf( h / 8.f ) ),
			srcTrait.m_depth
		};

		commandList.Dispatch( threadGroupCount[0], threadGroupCount[1], threadGroupCount[2] );

		return ApplyGaussianBlur<1.8f, 7>( esmsTexture );
	}

	agl::RefHandle<agl::Texture> GenerateExponentialShadowMaps( const ShadowInfo& shadowInfo, agl::RefHandle<agl::Texture> shadowMap )
	{
		if ( shadowMap.Get() == nullptr )
		{
			return shadowMap;
		}

		LightType lightType = shadowInfo.GetLightSceneInfo()->Proxy()->GetLightType();
		if ( lightType == LightType::Directional )
		{
			return GenerateCascadedESMs( shadowMap );
		}

		return shadowMap;
	}
}