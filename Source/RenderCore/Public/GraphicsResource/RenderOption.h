#pragma once

#include "GraphicsApiResource.h"
#include "HashUtil.h"
#include "Shader.h"
#include "SizedTypes.h"

#include <string>
#include <filesystem>

namespace rendercore
{
	struct RenderTargetBlendOption
	{
		bool m_blendEnable = false;
		agl::Blend m_srcBlend = agl::Blend::One;
		agl::Blend m_destBlend = agl::Blend::Zero;
		agl::BlendOp m_blendOp = agl::BlendOp::Add;
		agl::Blend m_srcBlendAlpha = agl::Blend::One;
		agl::Blend m_destBlendAlpha = agl::Blend::Zero;
		agl::BlendOp m_blendOpAlpha = agl::BlendOp::Add;
		agl::ColorWriteEnable m_renderTargetWriteMask = agl::ColorWriteEnable::All;

		friend bool operator==( const RenderTargetBlendOption& lhs, const RenderTargetBlendOption& rhs )
		{
			return lhs.m_blendEnable == rhs.m_blendEnable
				&& lhs.m_srcBlend == rhs.m_srcBlend
				&& lhs.m_destBlend == rhs.m_destBlend
				&& lhs.m_blendOp == rhs.m_blendOp
				&& lhs.m_srcBlendAlpha == rhs.m_srcBlendAlpha
				&& lhs.m_destBlendAlpha == rhs.m_destBlendAlpha
				&& lhs.m_blendOpAlpha == rhs.m_blendOpAlpha
				&& lhs.m_renderTargetWriteMask == rhs.m_renderTargetWriteMask;
		}

		friend RENDERCORE_DLL Archive& operator<<( Archive& ar, RenderTargetBlendOption& option );
	};

	struct RenderTargetBlendOptionHasher
	{
		size_t operator()( const RenderTargetBlendOption& option ) const
		{
			static size_t typeHash = typeid( RenderTargetBlendOption ).hash_code();
			size_t hash = typeHash;
			HashCombine( hash, option.m_blendEnable );
			HashCombine( hash, option.m_srcBlend );
			HashCombine( hash, option.m_destBlend );
			HashCombine( hash, option.m_blendOp );
			HashCombine( hash, option.m_srcBlendAlpha );
			HashCombine( hash, option.m_destBlendAlpha );
			HashCombine( hash, option.m_blendOpAlpha );
			HashCombine( hash, option.m_renderTargetWriteMask );

			return hash;
		}
	};

	class BlendOption : public AsyncLoadableAsset
	{
		GENERATE_CLASS_TYPE_INFO( BlendOption );
		DECLARE_ASSET( RENDERCORE, BlendOption );

	public:
		friend bool operator==( const BlendOption& lhs, const BlendOption& rhs )
		{
			bool ret = true;

			ret &= ( lhs.m_alphaToConverageEnable == rhs.m_alphaToConverageEnable );
			ret &= ( lhs.m_independentBlendEnable == rhs.m_independentBlendEnable );
			for ( uint32 i = 0; i < std::extent_v<decltype( lhs.m_renderTarget )>; ++i )
			{
				ret &= ( lhs.m_renderTarget[i] == rhs.m_renderTarget[i] );
			}
			ret &= ( lhs.m_sampleMask == rhs.m_sampleMask );

			return ret;
		}

		PROPERTY( alphaToConverageEnable )
		bool m_alphaToConverageEnable = false;

		PROPERTY( independentBlendEnable )
		bool m_independentBlendEnable = false;

		PROPERTY( renderTarget )
		RenderTargetBlendOption m_renderTarget[8];

		PROPERTY( sampleMask )
		uint32 m_sampleMask = ( std::numeric_limits<uint32>::max )( );

	protected:
		RENDERCORE_DLL virtual void PostLoadImpl() override;
	};

	struct BlendOptionHasher
	{
		size_t operator()( const BlendOption& option ) const
		{
			static size_t typeHash = typeid( BlendOption ).hash_code();
			size_t hash = typeHash;
			HashCombine( hash, option.m_alphaToConverageEnable );
			HashCombine( hash, option.m_independentBlendEnable );

			constexpr uint32 size = std::extent_v<decltype( option.m_renderTarget )>;
			for ( uint32 i = 0; i < size; ++i )
			{
				HashCombine( hash, i );
				HashCombine( hash, RenderTargetBlendOptionHasher()( option.m_renderTarget[i] ) );
			}

			HashCombine( hash, option.m_sampleMask );

			return hash;
		}
	};

	struct DepthOption
	{
		bool m_enable = true;
		bool m_writeDepth = true;
		agl::ComparisonFunc m_depthFunc = agl::ComparisonFunc::Less;

		friend bool operator==( const DepthOption& lhs, const DepthOption& rhs )
		{
			return lhs.m_enable == rhs.m_enable
				&& lhs.m_writeDepth == rhs.m_writeDepth
				&& lhs.m_depthFunc == rhs.m_depthFunc;
		}

		friend RENDERCORE_DLL Archive& operator<<( Archive& ar, DepthOption depthOption );
	};

	struct DepthOptionHasher
	{
		size_t operator()( const DepthOption& option ) const
		{
			static size_t typeHash = typeid( DepthOption ).hash_code();
			size_t hash = typeHash;
			HashCombine( hash, option.m_enable );
			HashCombine( hash, option.m_writeDepth );
			HashCombine( hash, option.m_depthFunc );

			return hash;
		}
	};

	struct StencilOption
	{
		bool m_enable = false;
		unsigned char m_readMask = 255;
		unsigned char m_writeMask = 255;
		agl::STENCIL_OP_TRAIT m_frontFace = { agl::StencilOp::Keep,
											agl::StencilOp::Keep,
											agl::StencilOp::Keep,
											agl::ComparisonFunc::Always };
		agl::STENCIL_OP_TRAIT m_backFace = { agl::StencilOp::Keep,
											agl::StencilOp::Keep,
											agl::StencilOp::Keep,
											agl::ComparisonFunc::Always };
		uint32 m_ref = 0;

		friend bool operator==( const StencilOption& lhs, const StencilOption& rhs )
		{
			return lhs.m_enable == rhs.m_enable
				&& lhs.m_readMask == rhs.m_readMask
				&& lhs.m_writeMask == rhs.m_writeMask
				&& lhs.m_frontFace == rhs.m_frontFace
				&& lhs.m_backFace == rhs.m_backFace
				&& lhs.m_ref == rhs.m_ref;
		}

		friend RENDERCORE_DLL Archive& operator<<( Archive& ar, StencilOption stencilOption );
	};

	struct StencilOptionHasher
	{
		size_t operator()( const StencilOption& option ) const
		{
			static size_t typeHash = typeid( StencilOption ).hash_code();
			size_t hash = typeHash;
			HashCombine( hash, option.m_enable );
			HashCombine( hash, option.m_readMask );
			HashCombine( hash, option.m_writeMask );
			HashCombine( hash, option.m_frontFace.m_failOp );
			HashCombine( hash, option.m_frontFace.m_depthFailOp );
			HashCombine( hash, option.m_frontFace.m_passOp );
			HashCombine( hash, option.m_frontFace.m_func );
			HashCombine( hash, option.m_backFace.m_failOp );
			HashCombine( hash, option.m_backFace.m_depthFailOp );
			HashCombine( hash, option.m_backFace.m_passOp );
			HashCombine( hash, option.m_backFace.m_func );
			HashCombine( hash, option.m_ref );

			return hash;
		}
	};

	class DepthStencilOption : public AsyncLoadableAsset
	{
		GENERATE_CLASS_TYPE_INFO( DepthStencilOption );
		DECLARE_ASSET( RENDERCORE, DepthStencilOption );

	public:
		friend bool operator==( const DepthStencilOption& lhs, const DepthStencilOption& rhs )
		{
			return lhs.m_depth == rhs.m_depth
				&& lhs.m_stencil == rhs.m_stencil;
		}

		PROPERTY( depth )
		DepthOption m_depth;

		PROPERTY( stencil )
		StencilOption m_stencil;

	protected:
		RENDERCORE_DLL virtual void PostLoadImpl() override;
	};

	struct DepthStencilOptionHasher
	{
		size_t operator()( const DepthStencilOption& option ) const
		{
			static size_t typeHash = typeid( DepthStencilOption ).hash_code();
			size_t hash = typeHash;
			HashCombine( hash, DepthOptionHasher()( option.m_depth ) );
			HashCombine( hash, StencilOptionHasher()( option.m_stencil ) );

			return hash;
		}
	};

	class RasterizerOption : public AsyncLoadableAsset
	{
		GENERATE_CLASS_TYPE_INFO( RasterizerOption );
		DECLARE_ASSET( RENDERCORE, RasterizerOption );

	public:
		friend bool operator==( const RasterizerOption& lhs, const RasterizerOption& rhs )
		{
			return lhs.m_isWireframe == rhs.m_isWireframe
				&& lhs.m_cullMode == rhs.m_cullMode
				&& lhs.m_counterClockwise == rhs.m_counterClockwise
				&& lhs.m_depthBias == rhs.m_depthBias
				&& lhs.m_depthClipEnable == rhs.m_depthClipEnable
				&& lhs.m_scissorEnable == rhs.m_scissorEnable
				&& lhs.m_multisampleEnable == rhs.m_multisampleEnable
				&& lhs.m_antialiasedLineEnable == rhs.m_antialiasedLineEnable;
		}

		PROPERTY( isWireframe )
		bool m_isWireframe = false;

		PROPERTY( cullMode )
		agl::CullMode m_cullMode = agl::CullMode::Back;

		PROPERTY( counterClockwise )
		bool m_counterClockwise = false;

		PROPERTY( depthBias )
		int32 m_depthBias = 0;

		PROPERTY( depthClipEnable )
		bool m_depthClipEnable = true;

		PROPERTY( scissorEnable )
		bool m_scissorEnable = false;

		PROPERTY( multisampleEnable)
		bool m_multisampleEnable = false;

		PROPERTY( antialiasedLineEnable )
		bool m_antialiasedLineEnable = false;

	protected:
		RENDERCORE_DLL virtual void PostLoadImpl() override;
	};

	struct RasterizerOptionHasher
	{
		size_t operator()( const RasterizerOption& option ) const
		{
			static size_t typeHash = typeid( RasterizerOption ).hash_code();
			size_t hash = typeHash;
			HashCombine( hash, option.m_isWireframe );
			HashCombine( hash, option.m_cullMode );
			HashCombine( hash, option.m_counterClockwise );
			HashCombine( hash, option.m_depthBias );
			HashCombine( hash, option.m_depthClipEnable );
			HashCombine( hash, option.m_scissorEnable );
			HashCombine( hash, option.m_multisampleEnable );
			HashCombine( hash, option.m_antialiasedLineEnable );

			return hash;
		}
	};

	class SamplerOption : public AsyncLoadableAsset
	{
		GENERATE_CLASS_TYPE_INFO( SamplerOption );
		DECLARE_ASSET( RENDERCORE, SamplerOption );

	public:
		friend bool operator==( const SamplerOption& lhs, const SamplerOption& rhs )
		{
			return lhs.m_filter == rhs.m_filter
				&& lhs.m_addressU == rhs.m_addressU
				&& lhs.m_addressV == rhs.m_addressV
				&& lhs.m_addressW == rhs.m_addressW
				&& lhs.m_mipLODBias == rhs.m_mipLODBias
				&& lhs.m_comparisonFunc == rhs.m_comparisonFunc;
		}

		PROPERTY( filter )
		agl::TextureFilter m_filter = agl::TextureFilter::MinLinear | agl::TextureFilter::MagLinear | agl::TextureFilter::MipLinear;

		PROPERTY( addressU )
		agl::TextureAddressMode m_addressU = agl::TextureAddressMode::Clamp;

		PROPERTY( addressV )
		agl::TextureAddressMode m_addressV = agl::TextureAddressMode::Clamp;

		PROPERTY( addressW )
		agl::TextureAddressMode m_addressW = agl::TextureAddressMode::Clamp;

		PROPERTY( mipLODBias )
		float m_mipLODBias = 0.f;

		PROPERTY( comparisonFunc )
		agl::ComparisonFunc m_comparisonFunc = agl::ComparisonFunc::Never;

	protected:
		RENDERCORE_DLL virtual void PostLoadImpl() override;
	};

	struct SamplerOptionHasher
	{
		size_t operator()( const SamplerOption& option ) const
		{
			static size_t typeHash = typeid( SamplerOption ).hash_code();
			size_t hash = typeHash;
			HashCombine( hash, option.m_filter );
			HashCombine( hash, option.m_addressU );
			HashCombine( hash, option.m_addressV );
			HashCombine( hash, option.m_addressW );
			HashCombine( hash, option.m_mipLODBias );
			HashCombine( hash, option.m_comparisonFunc );

			return hash;
		}
	};

	class RenderOption : public AsyncLoadableAsset
	{
		GENERATE_CLASS_TYPE_INFO( RenderOption );
		DECLARE_ASSET( RENDERCORE, RenderOption );

	public:
		friend bool operator==( const RenderOption& lhs, const RenderOption& rhs )
		{
			bool ret = true;

			ret &= ( lhs.m_blendOption == rhs.m_blendOption );
			ret &= ( lhs.m_depthStencilOption == rhs.m_depthStencilOption );
			ret &= ( lhs.m_rasterizerOption == rhs.m_rasterizerOption );

			return ret;
		}

		PROPERTY( blendOption )
		std::shared_ptr<BlendOption> m_blendOption = nullptr;

		PROPERTY( depthStencilOption )
		std::shared_ptr<DepthStencilOption> m_depthStencilOption = nullptr;

		PROPERTY( rasterizerOption )
		std::shared_ptr<RasterizerOption> m_rasterizerOption = nullptr;

	protected:
		RENDERCORE_DLL virtual void PostLoadImpl() override;
	};
}
