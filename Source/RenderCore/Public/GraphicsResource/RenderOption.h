#pragma once

#include "GraphicsApiResource.h"
#include "HashUtil.h"
#include "Shader.h"
#include "SizedTypes.h"

#include <string>
#include <filesystem>

struct RenderTargetBlendOption
{
	bool m_blendEnable = false;
	BLEND m_srcBlend = BLEND::ONE;
	BLEND m_destBlend = BLEND::ZERO;
	BLEND_OP m_blendOp = BLEND_OP::ADD;
	BLEND m_srcBlendAlpha = BLEND::ONE;
	BLEND m_destBlendAlpha = BLEND::ZERO;
	BLEND_OP m_blendOpAlpha = BLEND_OP::ADD;
	COLOR_WRITE_ENABLE m_renderTargetWriteMask = COLOR_WRITE_ENABLE::ALL;

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
	uint32 m_sampleMask = (std::numeric_limits<uint32>::max)();

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
	COMPARISON_FUNC m_depthFunc = COMPARISON_FUNC::LESS;

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
	STENCIL_OP_TRAIT m_frontFace = { STENCIL_OP::KEEP, 
									STENCIL_OP::KEEP, 
									STENCIL_OP::KEEP, 
									COMPARISON_FUNC::ALWAYS };
	STENCIL_OP_TRAIT m_backFace = { STENCIL_OP::KEEP, 
									STENCIL_OP::KEEP, 
									STENCIL_OP::KEEP, 
									COMPARISON_FUNC::ALWAYS };
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
			&& lhs.m_multisampleEnalbe == rhs.m_multisampleEnalbe
			&& lhs.m_antialiasedLineEnable == rhs.m_antialiasedLineEnable;
	}

	PROPERTY( isWireframe )
	bool m_isWireframe = false;

	PROPERTY( cullMode )
	CULL_MODE m_cullMode = CULL_MODE::BACK;

	PROPERTY( counterClockwise )
	bool m_counterClockwise = false;

	PROPERTY( depthBias )
	int32 m_depthBias = 0;

	PROPERTY( depthClipEnable )
	bool m_depthClipEnable = true;

	PROPERTY( scissorEnable )
	bool m_scissorEnable = false;

	PROPERTY( multisampleEnalbe )
	bool m_multisampleEnalbe = false;

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
		HashCombine( hash, option.m_multisampleEnalbe );
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
	uint32 m_filter = TEXTURE_FILTER::MIN_LINEAR | TEXTURE_FILTER::MAG_LINEAR | TEXTURE_FILTER::MIP_LINEAR;

	PROPERTY( addressU )
	TEXTURE_ADDRESS_MODE m_addressU = TEXTURE_ADDRESS_MODE::CLAMP;

	PROPERTY( addressV )
	TEXTURE_ADDRESS_MODE m_addressV = TEXTURE_ADDRESS_MODE::CLAMP;

	PROPERTY( addressW )
	TEXTURE_ADDRESS_MODE m_addressW = TEXTURE_ADDRESS_MODE::CLAMP;

	PROPERTY( mipLODBias )
	float m_mipLODBias = 0.f;

	PROPERTY( comparisonFunc )
	COMPARISON_FUNC m_comparisonFunc = COMPARISON_FUNC::NEVER;

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
