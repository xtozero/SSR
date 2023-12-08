#pragma once

#include "AbstractGraphicsInterface.h"
#include "ColorTypes.h"
#include "GraphicsApiResource.h"
#include "GraphicsPipelineState.h"
#include "SizedTypes.h"

namespace rendercore
{
	template<bool EnableBlend0 = false
		, agl::Blend SrcBlend0 = agl::Blend::One
		, agl::Blend DestBlend0 = agl::Blend::Zero
		, agl::BlendOp BlendOp0 = agl::BlendOp::Add
		, agl::Blend SrcBlendAlpha0 = agl::Blend::One
		, agl::Blend DestBlendAlpha0 = agl::Blend::Zero
		, agl::BlendOp BlendOpAlpha0 = agl::BlendOp::Add
		, agl::ColorWriteEnable WriteMask0 = agl::ColorWriteEnable::All
		, bool EnableAlphaToConverage = false
		, bool EnableIndependentBlend = false
		, uint32 SampleMask = ( std::numeric_limits<uint32>::max )( )
		, bool EnableBlend1 = false
		, agl::Blend SrcBlend1 = agl::Blend::One
		, agl::Blend DestBlend1 = agl::Blend::Zero
		, agl::BlendOp BlendOp1 = agl::BlendOp::Add
		, agl::Blend SrcBlendAlpha1 = agl::Blend::One
		, agl::Blend DestBlendAlpha1 = agl::Blend::Zero
		, agl::BlendOp BlendOpAlpha1 = agl::BlendOp::Add
		, agl::ColorWriteEnable WriteMask1 = agl::ColorWriteEnable::All
		, bool EnableBlend2 = false
		, agl::Blend SrcBlend2 = agl::Blend::One
		, agl::Blend DestBlend2 = agl::Blend::Zero
		, agl::BlendOp BlendOp2 = agl::BlendOp::Add
		, agl::Blend SrcBlendAlpha2 = agl::Blend::One
		, agl::Blend DestBlendAlpha2 = agl::Blend::Zero
		, agl::BlendOp BlendOpAlpha2 = agl::BlendOp::Add
		, agl::ColorWriteEnable WriteMask2 = agl::ColorWriteEnable::All
		, bool EnableBlend3 = false
		, agl::Blend SrcBlend3 = agl::Blend::One
		, agl::Blend DestBlend3 = agl::Blend::Zero
		, agl::BlendOp BlendOp3 = agl::BlendOp::Add
		, agl::Blend SrcBlendAlpha3 = agl::Blend::One
		, agl::Blend DestBlendAlpha3 = agl::Blend::Zero
		, agl::BlendOp BlendOpAlpha3 = agl::BlendOp::Add
		, agl::ColorWriteEnable WriteMask3 = agl::ColorWriteEnable::All
		, bool EnableBlend4 = false
		, agl::Blend SrcBlend4 = agl::Blend::One
		, agl::Blend DestBlend4 = agl::Blend::Zero
		, agl::BlendOp BlendOp4 = agl::BlendOp::Add
		, agl::Blend SrcBlendAlpha4 = agl::Blend::One
		, agl::Blend DestBlendAlpha4 = agl::Blend::Zero
		, agl::BlendOp BlendOpAlpha4 = agl::BlendOp::Add
		, agl::ColorWriteEnable WriteMask4 = agl::ColorWriteEnable::All
		, bool EnableBlend5 = false
		, agl::Blend SrcBlend5 = agl::Blend::One
		, agl::Blend DestBlend5 = agl::Blend::Zero
		, agl::BlendOp BlendOp5 = agl::BlendOp::Add
		, agl::Blend SrcBlendAlpha5 = agl::Blend::One
		, agl::Blend DestBlendAlpha5 = agl::Blend::Zero
		, agl::BlendOp BlendOpAlpha5 = agl::BlendOp::Add
		, agl::ColorWriteEnable WriteMask5 = agl::ColorWriteEnable::All
		, bool EnableBlend6 = false
		, agl::Blend SrcBlend6 = agl::Blend::One
		, agl::Blend DestBlend6 = agl::Blend::Zero
		, agl::BlendOp BlendOp6 = agl::BlendOp::Add
		, agl::Blend SrcBlendAlpha6 = agl::Blend::One
		, agl::Blend DestBlendAlpha6 = agl::Blend::Zero
		, agl::BlendOp BlendOpAlpha6 = agl::BlendOp::Add
		, agl::ColorWriteEnable WriteMask6 = agl::ColorWriteEnable::All
		, bool EnableBlend7 = false
		, agl::Blend SrcBlend7 = agl::Blend::One
		, agl::Blend DestBlend7 = agl::Blend::Zero
		, agl::BlendOp BlendOp7 = agl::BlendOp::Add
		, agl::Blend SrcBlendAlpha7 = agl::Blend::One
		, agl::Blend DestBlendAlpha7 = agl::Blend::Zero
		, agl::BlendOp BlendOpAlpha7 = agl::BlendOp::Add
		, agl::ColorWriteEnable WriteMask7 = agl::ColorWriteEnable::All>
	class StaticBlendState final
	{
	public:
		static BlendState Resource()
		{
			if ( State == nullptr )
			{
				BlendOption option;
				option.m_alphaToConverageEnable = EnableAlphaToConverage;
				option.m_independentBlendEnable = EnableIndependentBlend;
				option.m_sampleMask = SampleMask;
				option.m_renderTarget[0].m_blendEnable = EnableBlend0;
				option.m_renderTarget[0].m_srcBlend = SrcBlend0;
				option.m_renderTarget[0].m_destBlend = DestBlend0;
				option.m_renderTarget[0].m_blendOp = BlendOp0;
				option.m_renderTarget[0].m_srcBlendAlpha = SrcBlendAlpha0;
				option.m_renderTarget[0].m_destBlendAlpha = DestBlendAlpha0;
				option.m_renderTarget[0].m_blendOpAlpha = BlendOpAlpha0;
				option.m_renderTarget[0].m_renderTargetWriteMask = WriteMask0;

				option.m_renderTarget[1].m_blendEnable = EnableBlend1;
				option.m_renderTarget[1].m_srcBlend = SrcBlend1;
				option.m_renderTarget[1].m_destBlend = DestBlend1;
				option.m_renderTarget[1].m_blendOp = BlendOp1;
				option.m_renderTarget[1].m_srcBlendAlpha = SrcBlendAlpha1;
				option.m_renderTarget[1].m_destBlendAlpha = DestBlendAlpha1;
				option.m_renderTarget[1].m_blendOpAlpha = BlendOpAlpha1;
				option.m_renderTarget[1].m_renderTargetWriteMask = WriteMask1;

				option.m_renderTarget[2].m_blendEnable = EnableBlend2;
				option.m_renderTarget[2].m_srcBlend = SrcBlend2;
				option.m_renderTarget[2].m_destBlend = DestBlend2;
				option.m_renderTarget[2].m_blendOp = BlendOp2;
				option.m_renderTarget[2].m_srcBlendAlpha = SrcBlendAlpha2;
				option.m_renderTarget[2].m_destBlendAlpha = DestBlendAlpha2;
				option.m_renderTarget[2].m_blendOpAlpha = BlendOpAlpha2;
				option.m_renderTarget[2].m_renderTargetWriteMask = WriteMask2;

				option.m_renderTarget[3].m_blendEnable = EnableBlend3;
				option.m_renderTarget[3].m_srcBlend = SrcBlend3;
				option.m_renderTarget[3].m_destBlend = DestBlend3;
				option.m_renderTarget[3].m_blendOp = BlendOp3;
				option.m_renderTarget[3].m_srcBlendAlpha = SrcBlendAlpha3;
				option.m_renderTarget[3].m_destBlendAlpha = DestBlendAlpha3;
				option.m_renderTarget[3].m_blendOpAlpha = BlendOpAlpha3;
				option.m_renderTarget[3].m_renderTargetWriteMask = WriteMask3;

				option.m_renderTarget[4].m_blendEnable = EnableBlend4;
				option.m_renderTarget[4].m_srcBlend = SrcBlend4;
				option.m_renderTarget[4].m_destBlend = DestBlend4;
				option.m_renderTarget[4].m_blendOp = BlendOp4;
				option.m_renderTarget[4].m_srcBlendAlpha = SrcBlendAlpha4;
				option.m_renderTarget[4].m_destBlendAlpha = DestBlendAlpha4;
				option.m_renderTarget[4].m_blendOpAlpha = BlendOpAlpha4;
				option.m_renderTarget[4].m_renderTargetWriteMask = WriteMask4;

				option.m_renderTarget[5].m_blendEnable = EnableBlend5;
				option.m_renderTarget[5].m_srcBlend = SrcBlend5;
				option.m_renderTarget[5].m_destBlend = DestBlend5;
				option.m_renderTarget[5].m_blendOp = BlendOp5;
				option.m_renderTarget[5].m_srcBlendAlpha = SrcBlendAlpha5;
				option.m_renderTarget[5].m_destBlendAlpha = DestBlendAlpha5;
				option.m_renderTarget[5].m_blendOpAlpha = BlendOpAlpha5;
				option.m_renderTarget[5].m_renderTargetWriteMask = WriteMask5;

				option.m_renderTarget[6].m_blendEnable = EnableBlend6;
				option.m_renderTarget[6].m_srcBlend = SrcBlend6;
				option.m_renderTarget[6].m_destBlend = DestBlend6;
				option.m_renderTarget[6].m_blendOp = BlendOp6;
				option.m_renderTarget[6].m_srcBlendAlpha = SrcBlendAlpha6;
				option.m_renderTarget[6].m_destBlendAlpha = DestBlendAlpha6;
				option.m_renderTarget[6].m_blendOpAlpha = BlendOpAlpha6;
				option.m_renderTarget[6].m_renderTargetWriteMask = WriteMask6;

				option.m_renderTarget[7].m_blendEnable = EnableBlend7;
				option.m_renderTarget[7].m_srcBlend = SrcBlend7;
				option.m_renderTarget[7].m_destBlend = DestBlend7;
				option.m_renderTarget[7].m_blendOp = BlendOp7;
				option.m_renderTarget[7].m_srcBlendAlpha = SrcBlendAlpha7;
				option.m_renderTarget[7].m_destBlendAlpha = DestBlendAlpha7;
				option.m_renderTarget[7].m_blendOpAlpha = BlendOpAlpha7;
				option.m_renderTarget[7].m_renderTargetWriteMask = WriteMask7;

				State = GraphicsInterface().FindOrCreate( option ).Resource();
			}

			return BlendState( State );
		}

	private:
		static agl::BlendState* State;
	};

	template<bool EnableBlend0
		, agl::Blend SrcBlend0
		, agl::Blend DestBlend0
		, agl::BlendOp BlendOp0
		, agl::Blend SrcBlendAlpha0
		, agl::Blend DestBlendAlpha0
		, agl::BlendOp BlendOpAlpha0
		, agl::ColorWriteEnable WriteMask0
		, bool EnableAlphaToConverage
		, bool EnableIndependentBlend
		, uint32 SampleMask
		, bool EnableBlend1
		, agl::Blend SrcBlend1
		, agl::Blend DestBlend1
		, agl::BlendOp BlendOp1
		, agl::Blend SrcBlendAlpha1
		, agl::Blend DestBlendAlpha1
		, agl::BlendOp BlendOpAlpha1
		, agl::ColorWriteEnable WriteMask1
		, bool EnableBlend2
		, agl::Blend SrcBlend2
		, agl::Blend DestBlend2
		, agl::BlendOp BlendOp2
		, agl::Blend SrcBlendAlpha2
		, agl::Blend DestBlendAlpha2
		, agl::BlendOp BlendOpAlpha2
		, agl::ColorWriteEnable WriteMask2
		, bool EnableBlend3
		, agl::Blend SrcBlend3
		, agl::Blend DestBlend3
		, agl::BlendOp BlendOp3
		, agl::Blend SrcBlendAlpha3
		, agl::Blend DestBlendAlpha3
		, agl::BlendOp BlendOpAlpha3
		, agl::ColorWriteEnable WriteMask3
		, bool EnableBlend4
		, agl::Blend SrcBlend4
		, agl::Blend DestBlend4
		, agl::BlendOp BlendOp4
		, agl::Blend SrcBlendAlpha4
		, agl::Blend DestBlendAlpha4
		, agl::BlendOp BlendOpAlpha4
		, agl::ColorWriteEnable WriteMask4
		, bool EnableBlend5
		, agl::Blend SrcBlend5
		, agl::Blend DestBlend5
		, agl::BlendOp BlendOp5
		, agl::Blend SrcBlendAlpha5
		, agl::Blend DestBlendAlpha5
		, agl::BlendOp BlendOpAlpha5
		, agl::ColorWriteEnable WriteMask5
		, bool EnableBlend6
		, agl::Blend SrcBlend6
		, agl::Blend DestBlend6
		, agl::BlendOp BlendOp6
		, agl::Blend SrcBlendAlpha6
		, agl::Blend DestBlendAlpha6
		, agl::BlendOp BlendOpAlpha6
		, agl::ColorWriteEnable WriteMask6
		, bool EnableBlend7
		, agl::Blend SrcBlend7
		, agl::Blend DestBlend7
		, agl::BlendOp BlendOp7
		, agl::Blend SrcBlendAlpha7
		, agl::Blend DestBlendAlpha7
		, agl::BlendOp BlendOpAlpha7
		, agl::ColorWriteEnable WriteMask7>
	agl::BlendState* StaticBlendState<EnableBlend0, SrcBlend0, DestBlend0, BlendOp0, SrcBlendAlpha0, DestBlendAlpha0, BlendOpAlpha0, WriteMask0, EnableAlphaToConverage, EnableIndependentBlend, SampleMask, EnableBlend1, SrcBlend1, DestBlend1, BlendOp1, SrcBlendAlpha1, DestBlendAlpha1, BlendOpAlpha1, WriteMask1, EnableBlend2, SrcBlend2, DestBlend2, BlendOp2, SrcBlendAlpha2, DestBlendAlpha2, BlendOpAlpha2, WriteMask2, EnableBlend3, SrcBlend3, DestBlend3, BlendOp3, SrcBlendAlpha3, DestBlendAlpha3, BlendOpAlpha3, WriteMask3, EnableBlend4, SrcBlend4, DestBlend4, BlendOp4, SrcBlendAlpha4, DestBlendAlpha4, BlendOpAlpha4, WriteMask4, EnableBlend5, SrcBlend5, DestBlend5, BlendOp5, SrcBlendAlpha5, DestBlendAlpha5, BlendOpAlpha5, WriteMask5, EnableBlend6, SrcBlend6, DestBlend6, BlendOp6, SrcBlendAlpha6, DestBlendAlpha6, BlendOpAlpha6, WriteMask6, EnableBlend7, SrcBlend7, DestBlend7, BlendOp7, SrcBlendAlpha7, DestBlendAlpha7, BlendOpAlpha7, WriteMask7>::State = nullptr;

	template<bool EnableDepth = true
		, bool WriteDepth = true
		, agl::ComparisonFunc DepthTest = agl::ComparisonFunc::Less
		, bool EnableStencil = false
		, agl::ComparisonFunc FrontFaceStencilTest = agl::ComparisonFunc::Always
		, agl::StencilOp FrontFaceFail = agl::StencilOp::Keep
		, agl::StencilOp FrontFaceDepthFail = agl::StencilOp::Keep
		, agl::StencilOp FrontFacePass = agl::StencilOp::Keep
		, agl::ComparisonFunc BackFaceStencilTest = agl::ComparisonFunc::Always
		, agl::StencilOp BackFaceFail = agl::StencilOp::Keep
		, agl::StencilOp BackFaceDepthFail = agl::StencilOp::Keep
		, agl::StencilOp BackFacePass = agl::StencilOp::Keep
		, uint8 ReadMask = 0xFF
		, uint8 WriteMask = 0xFF>
	class StaticDepthStencilState final
	{
	public:
		static DepthStencilState Get()
		{
			if ( State == nullptr )
			{
				DepthStencilOption option;
				option.m_depth = {
					.m_enable = EnableDepth,
					.m_writeDepth = WriteDepth,
					.m_depthFunc = DepthTest,
				};
				option.m_stencil = {
					.m_enable = EnableStencil,
					.m_readMask = ReadMask,
					.m_writeMask = WriteMask,
					.m_frontFace = {
						.m_failOp = FrontFaceFail,
						.m_depthFailOp = FrontFaceDepthFail,
						.m_passOp = FrontFacePass,
						.m_func = FrontFaceStencilTest,
					},
					.m_backFace = {
						.m_failOp = BackFaceFail,
						.m_depthFailOp = BackFaceDepthFail,
						.m_passOp = BackFacePass,
						.m_func = BackFaceStencilTest,
					},
				};

				State = GraphicsInterface().FindOrCreate( option ).Resource();
			}

			return DepthStencilState( State );
		}

	private:
		static agl::DepthStencilState* State;
	};

	template<bool EnableDepth
		, bool WriteDepth
		, agl::ComparisonFunc DepthTest
		, bool EnableStencil
		, agl::ComparisonFunc FrontFaceStencilTest
		, agl::StencilOp FrontFaceFail
		, agl::StencilOp FrontFaceDepthFail
		, agl::StencilOp FrontFacePass
		, agl::ComparisonFunc BackFaceStencilTest
		, agl::StencilOp BackFaceFail
		, agl::StencilOp BackFaceDepthFail
		, agl::StencilOp BackFacePass
		, uint8 ReadMask
		, uint8 WriteMask>
	agl::DepthStencilState* StaticDepthStencilState<EnableDepth, WriteDepth, DepthTest, EnableStencil, FrontFaceStencilTest, FrontFaceFail, FrontFaceDepthFail, FrontFacePass, BackFaceStencilTest, BackFaceFail, BackFaceDepthFail, BackFacePass, ReadMask, WriteMask>::State = nullptr;

	template<bool WireFrame = false
		, agl::CullMode CullMode = agl::CullMode::Back
		, bool CounterClockwise = false
		, int32 DepthBias = 0
		, bool EnableDepthClip = true
		, bool EnableScissor = false
		, bool EnableMultiSample = false
		, bool EnableAntialiasedLine = false>
	class StaticRasterizerState final
	{
	public:
		static RasterizerState Get()
		{
			if ( State == nullptr )
			{
				RasterizerOption option;
				option.m_isWireframe = WireFrame;
				option.m_cullMode = CullMode;
				option.m_counterClockwise = CounterClockwise;
				option.m_depthBias = DepthBias;
				option.m_depthClipEnable = EnableDepthClip;
				option.m_scissorEnable = EnableScissor;
				option.m_multisampleEnable = EnableMultiSample;
				option.m_antialiasedLineEnable = EnableAntialiasedLine;

				State = GraphicsInterface().FindOrCreate( option ).Resource();
			}

			return RasterizerState( State );
		}

	private:
		static agl::RasterizerState* State;
	};

	template<bool WireFrame
		, agl::CullMode CullMode
		, bool CounterClockwise
		, int32 DepthBias
		, bool EnableDepthClip
		, bool EnableScissor
		, bool EnableMultiSample
		, bool EnableAntialiasedLine>
	agl::RasterizerState* StaticRasterizerState<WireFrame, CullMode, CounterClockwise, DepthBias, EnableDepthClip, EnableScissor, EnableMultiSample, EnableAntialiasedLine>::State = nullptr;

	template<agl::TextureFilter Filter = agl::TextureFilter::MinMagMipLinear
		, agl::TextureAddressMode AddressU = agl::TextureAddressMode::Clamp
		, agl::TextureAddressMode AddressV = agl::TextureAddressMode::Clamp
		, agl::TextureAddressMode AddressW = agl::TextureAddressMode::Clamp
		, float MipLODBias = 0.f
		, agl::ComparisonFunc ComparisonFunc = agl::ComparisonFunc::Never
		, Color BorderColor = Color(255, 255, 255, 255)>
	class StaticSamplerState final
	{
	public:
		static SamplerState Get()
		{
			if ( State == nullptr )
			{
				SamplerOption option;
				option.m_filter = Filter;
				option.m_addressU = AddressU;
				option.m_addressV = AddressV;
				option.m_addressW = AddressW;
				option.m_mipLODBias = MipLODBias;
				option.m_comparisonFunc = ComparisonFunc;
				option.m_borderColor = BorderColor;

				State = GraphicsInterface().FindOrCreate( option ).Resource();
			}

			return SamplerState( State );
		}

	private:
		static agl::SamplerState* State;
	};

	template<agl::TextureFilter Filter
		, agl::TextureAddressMode AddressU
		, agl::TextureAddressMode AddressV
		, agl::TextureAddressMode AddressW
		, float MipLODBias
		, agl::ComparisonFunc ComparisonFunc
		, Color BorderColor>
	agl::SamplerState* StaticSamplerState<Filter, AddressU, AddressV, AddressW, MipLODBias, ComparisonFunc, BorderColor>::State = nullptr;
}