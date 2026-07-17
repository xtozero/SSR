#include "ExponentialShadowMapRendering.h"

#include "CommandList.h"
#include "ComputePipelineState.h"
#include "Config/DefaultRenderCoreConfig.h"
#include "GlobalShaders.h"
#include "InlineMemoryAllocator.h"
#include "Multithread/TaskScheduler.h"
#include "Proxies/LightProxy.h"
#include "RenderGraph.h"
#include "Scene/LightSceneInfo.h"
#include "Scene/ShadowInfo.h"
#include "ShaderParameterUtils.h"
#include "StaticState.h"

#include <cassert>
#include <numbers>

namespace
{
	float Gaussian( int32 x, float sigma )
	{
		constexpr float Pi = std::numbers::pi_v<float>;
		return 1.f / ( std::sqrtf( 2.f * Pi ) * sigma ) * std::expf( -( x * x ) / ( 2.f * sigma * sigma ) );
	}
}

namespace rendercore
{
	class VerticalDim : DEFINE_BOOL_DIMENSION( "Vertical" );
	class KernelSizeDim : DEFINE_RANGE_DIMENSION( "KernelSize", 0, 127 );

	class CascadedESMsBlurCS final : public GlobalShaderBase<ComputeShader, CascadedESMsBlurCS>
	{
		using GlobalShaderBase::GlobalShaderBase;

	public:
		using PermutationType = ShaderPermutation<VerticalDim, KernelSizeDim>;

		CascadedESMsBlurCS() = default;

	private:
		DECLARE_SHADER_PARAM( KernelBuffer );
		DECLARE_SHADER_PARAM( SrcTexture );
		DECLARE_SHADER_PARAM( PointSampler );
		DECLARE_SHADER_PARAM( DestTexture );
	};

	class CascadedESMsCS final : public GlobalShaderBase<ComputeShader, CascadedESMsCS>
	{
		DECLARE_SHADER_PARAM( SrcTexture );
		DECLARE_SHADER_PARAM( ESMsTexture );
		DECLARE_SHADER_PARAM( ParameterC );
	};

	REGISTER_GLOBAL_SHADER( CascadedESMsBlurCS, "Shadow/CS_CascadedESMsBlur.fx", "main" );
	REGISTER_GLOBAL_SHADER( CascadedESMsCS, "Shadow/CS_CascadedESMs.fx", "main" );

	template <float Sigma, int32 KernelSize>
	RefHandle<agl::Texture> ApplyGaussianBlur( RenderGraph& renderGraph, RefHandle<agl::Texture> srcTexture )
	{
		static_assert( KernelSize < 128, "Maximum kernel size exceeded" );

		float kernel[128] = {};
		for ( int32 x = -KernelSize / 2, i = 0; x <= KernelSize / 2; ++x )
		{
			kernel[i++] = Gaussian( x, Sigma );
		}

		auto rgSource = renderGraph.RegisterExternalResource( srcTexture.Get() );

		const agl::TextureDesc& srcDesc = srcTexture->GetDesc();
		agl::TextureDesc tempDesc = srcDesc;
		tempDesc.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess;
		auto rgTemp = renderGraph.CreateTexture( tempDesc, "Blur.Temp" );

		BEGIN_RG_RESOURCE_STRUCT( BlurPassResource )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( input )
			DECLARE_RG_TEXTURE_UAV( output )
		END_RG_RESOURCE_STRUCT();

		BlurPassResource horizonBlurPassResource = {
			.m_input = rgSource,
			.m_output = rgTemp
		};

		renderGraph.AddPass(
			horizonBlurPassResource,
			[horizonBlurPassResource, srcDesc, kernel]( ComputeCommandList& commandList )
			{
				CascadedESMsBlurCS::PermutationType permutation;
				permutation.SetValue<KernelSizeDim>( KernelSize );

				CascadedESMsBlurCS horizonBlurCS( permutation );
				RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( horizonBlurCS );

				commandList.BindPipelineState( pso.Get() );

				SamplerState pointSampler = StaticSamplerState<agl::TextureFilter::Point>::Get();

				agl::ShaderBindings shaderBindings = CreateShaderBindings( horizonBlurCS );
				BindResource( shaderBindings, horizonBlurCS.SrcTexture(), horizonBlurPassResource.m_input->Get() );
				BindResource( shaderBindings, horizonBlurCS.PointSampler(), pointSampler );
				BindResource( shaderBindings, horizonBlurCS.DestTexture(), horizonBlurPassResource.m_output->Get() );

				SetShaderValue( commandList, horizonBlurCS.KernelBuffer(), kernel );

				commandList.BindShaderResources( shaderBindings );

				uint32 w = srcDesc.m_width;
				uint32 h = srcDesc.m_height;

				assert( ( w % 8 == 0 ) && ( h % 8 == 0 ) );

				const uint32 threadGroupCount[3] = {
					static_cast<uint32>( std::ceilf( w / 8.f ) ),
					static_cast<uint32>( std::ceilf( h / 8.f ) ),
					srcDesc.m_depth
				};

				commandList.Dispatch( threadGroupCount[0], threadGroupCount[1], threadGroupCount[2] );
			} );

		BlurPassResource verticalBlurPassResource = {
			.m_input = rgTemp,
			.m_output = rgSource
		};

		renderGraph.AddPass(
			verticalBlurPassResource,
			[verticalBlurPassResource, srcDesc, kernel]( ComputeCommandList& commandList )
			{
				CascadedESMsBlurCS::PermutationType permutation;
				permutation.SetValue<VerticalDim>( 1 );
				permutation.SetValue<KernelSizeDim>( KernelSize );

				CascadedESMsBlurCS verticalBlurCS( permutation );
				RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( verticalBlurCS );

				commandList.BindPipelineState( pso.Get() );

				SamplerState pointSampler = StaticSamplerState<agl::TextureFilter::Point>::Get();

				agl::ShaderBindings shaderBindings = CreateShaderBindings( verticalBlurCS );
				BindResource( shaderBindings, verticalBlurCS.SrcTexture(), verticalBlurPassResource.m_input->Get() );
				BindResource( shaderBindings, verticalBlurCS.PointSampler(), pointSampler );
				BindResource( shaderBindings, verticalBlurCS.DestTexture(), verticalBlurPassResource.m_output->Get() );

				SetShaderValue( commandList, verticalBlurCS.KernelBuffer(), kernel );

				commandList.BindShaderResources( shaderBindings );

				uint32 w = srcDesc.m_width;
				uint32 h = srcDesc.m_height;

				assert( ( w % 8 == 0 ) && ( h % 8 == 0 ) );

				const uint32 threadGroupCount[3] = {
					static_cast<uint32>( std::ceilf( w / 8.f ) ),
					static_cast<uint32>( std::ceilf( h / 8.f ) ),
					srcDesc.m_depth
				};

				commandList.Dispatch( threadGroupCount[0], threadGroupCount[1], threadGroupCount[2] );
			} );

		return srcTexture;
	}

	RefHandle<agl::Texture> GenerateCascadedESMs( RenderGraph& renderGraph, RefHandle<agl::Texture> shadowMap )
	{
		assert( IsInRenderThread() );

		const agl::TextureDesc& srcDesc = shadowMap->GetDesc();
		agl::TextureDesc esmsDesc = srcDesc;
		esmsDesc.m_format = agl::ResourceFormat::R32_FLOAT;
		esmsDesc.m_bindType = agl::ResourceBindType::ShaderResource | agl::ResourceBindType::RandomAccess;

		RefHandle<agl::Texture> esmsTexture = agl::Texture::Create( esmsDesc, "ESMs" );

		auto rgShadowMap = renderGraph.RegisterExternalResource( shadowMap.Get() );
		auto rgEsms = renderGraph.RegisterExternalResource( esmsTexture.Get() );

		BEGIN_RG_RESOURCE_STRUCT( ESMsPassResource )
			DECLARE_RG_TEXTURE_NONPIXEL_SRV( shadowMap )
			DECLARE_RG_TEXTURE_UAV( esms )
		END_RG_RESOURCE_STRUCT();

		ESMsPassResource passResource = {
			.m_shadowMap = rgShadowMap,
			.m_esms = rgEsms
		};

		renderGraph.AddPass(
			passResource,
			[passResource, srcDesc]( ComputeCommandList& commandList )
			{
				CascadedESMsCS cascadedESMsCS;

				RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( cascadedESMsCS );

				commandList.BindPipelineState( pso.Get() );

				agl::ShaderBindings shaderBindings = CreateShaderBindings( cascadedESMsCS );
				BindResource( shaderBindings, cascadedESMsCS.SrcTexture(), passResource.m_shadowMap->Get() );
				BindResource( shaderBindings, cascadedESMsCS.ESMsTexture(), passResource.m_esms->Get() );

				SetShaderValue( commandList, cascadedESMsCS.ParameterC(), DefaultRenderCore::ESMsParamC() );

				commandList.BindShaderResources( shaderBindings );

				uint32 w = srcDesc.m_width;
				uint32 h = srcDesc.m_height;

				assert( ( w % 8 == 0 ) && ( h % 8 == 0 ) );

				const uint32 threadGroupCount[3] = {
					static_cast<uint32>( std::ceilf( w / 8.f ) ),
					static_cast<uint32>( std::ceilf( h / 8.f ) ),
					srcDesc.m_depth
				};

				commandList.Dispatch( threadGroupCount[0], threadGroupCount[1], threadGroupCount[2] );
			} );

		return ApplyGaussianBlur<1.8f, 7>( renderGraph, esmsTexture );
	}

	RefHandle<agl::Texture> GenerateExponentialShadowMaps( RenderGraph& renderGraph, const ShadowInfo& shadowInfo, RefHandle<agl::Texture> shadowMap )
	{
		if ( shadowMap.Get() == nullptr )
		{
			return shadowMap;
		}

		LightType lightType = shadowInfo.GetLightType();
		if ( lightType == LightType::Directional )
		{
			return GenerateCascadedESMs( renderGraph, shadowMap );
		}

		return shadowMap;
	}
}