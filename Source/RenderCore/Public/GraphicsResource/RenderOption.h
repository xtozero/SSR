#pragma once

#include "GraphicsApiResource.h"
#include "HashUtil.h"
#include "Shader.h"

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
};

struct RenderTargetBlendOptionHasher
{
	std::size_t operator()( const RenderTargetBlendOption& option ) const
	{
		std::size_t hash = typeid( RenderTargetBlendOption ).hash_code( );
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
	DECLARE_ASSET( RENDERCORE, BlendOption );
public:
	RENDERCORE_DLL virtual void Serialize( Archive& ar ) override;

	const std::filesystem::path& Path( ) const { return m_path; }
	void SetPath( const std::filesystem::path& path ) { m_path = path; }

	friend bool operator==( const BlendOption& lhs, const BlendOption& rhs )
	{
		bool ret = true;

		ret &= ( lhs.m_alphaToConverageEnable == rhs.m_alphaToConverageEnable );
		ret &= ( lhs.m_independentBlendEnable == rhs.m_independentBlendEnable );
		for ( int i = 0; i < std::extent_v<decltype( lhs.m_renderTarget )>; ++i )
		{
			ret &= ( lhs.m_renderTarget[i] == rhs.m_renderTarget[i] );
		}

		return ret;
	}

	bool m_alphaToConverageEnable;
	bool m_independentBlendEnable;
	RenderTargetBlendOption m_renderTarget[8];

protected:
	RENDERCORE_DLL virtual void PostLoadImpl( ) override;

private:
	std::filesystem::path m_path;
};

struct BlendOptionHasher
{
	std::size_t operator()( const BlendOption& option ) const
	{
		std::size_t hash = typeid( BlendOption ).hash_code( );
		HashCombine( hash, option.m_alphaToConverageEnable );
		HashCombine( hash, option.m_independentBlendEnable );
		
		constexpr int size = std::extent_v<decltype( option.m_renderTarget )>;
		for ( int i = 0; i < size; ++i )
		{
			HashCombine( hash, i );
			HashCombine( hash, RenderTargetBlendOptionHasher( )( option.m_renderTarget[i] ) );
		}

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
};

struct DepthOptionHasher
{
	std::size_t operator()( const DepthOption& option ) const
	{
		std::size_t hash = typeid( DepthOption ).hash_code();
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
	unsigned int m_ref = 0;

	friend bool operator==( const StencilOption& lhs, const StencilOption& rhs )
	{
		return lhs.m_enable == rhs.m_enable
			&& lhs.m_readMask == rhs.m_readMask
			&& lhs.m_writeMask == rhs.m_writeMask
			&& lhs.m_frontFace == rhs.m_frontFace
			&& lhs.m_backFace == rhs.m_backFace
			&& lhs.m_ref == rhs.m_ref;
	}
};

struct StencilOptionHasher
{
	std::size_t operator()( const StencilOption& option ) const
	{
		std::size_t hash = typeid( StencilOption ).hash_code( );
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
	DECLARE_ASSET( RENDERCORE, DepthStencilOption );
public:
	RENDERCORE_DLL virtual void Serialize( Archive& ar ) override;

	const std::filesystem::path& Path( ) const { return m_path; }
	void SetPath( const std::filesystem::path& path ) { m_path = path; }

	friend bool operator==( const DepthStencilOption& lhs, const DepthStencilOption& rhs )
	{
		return lhs.m_depth == rhs.m_depth
			&& lhs.m_stencil == rhs.m_stencil;
	}

	DepthOption m_depth;
	StencilOption m_stencil;

protected:
	RENDERCORE_DLL virtual void PostLoadImpl( ) override;

private:
	std::filesystem::path m_path;
};

struct DepthStencilOptionHasher
{
	std::size_t operator()( const DepthStencilOption& option ) const
	{
		std::size_t hash = typeid( DepthStencilOption ).hash_code( );
		HashCombine( hash, DepthOptionHasher( )( option.m_depth ) );
		HashCombine( hash, StencilOptionHasher( )( option.m_stencil ) );

		return hash;
	}
};

class RasterizerOption : public AsyncLoadableAsset
{
	DECLARE_ASSET( RENDERCORE, RasterizerOption );
public:
	RENDERCORE_DLL virtual void Serialize( Archive& ar ) override;

	const std::filesystem::path& Path( ) const { return m_path; }
	void SetPath( const std::filesystem::path& path ) { m_path = path; }

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

	bool m_isWireframe = false;
	CULL_MODE m_cullMode = CULL_MODE::BACK;
	bool m_counterClockwise = false;
	int m_depthBias = 0;
	bool m_depthClipEnable = true;
	bool m_scissorEnable = false;
	bool m_multisampleEnalbe = false;
	bool m_antialiasedLineEnable = false;

protected:
	RENDERCORE_DLL virtual void PostLoadImpl( ) override;

private:
	std::filesystem::path m_path;
};

struct RasterizerOptionHasher
{
	std::size_t operator()( const RasterizerOption& option ) const
	{
		std::size_t hash = typeid( RasterizerOption ).hash_code( );
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
	DECLARE_ASSET( RENDERCORE, SamplerOption );
public:
	RENDERCORE_DLL virtual void Serialize( Archive& ar ) override;

	const std::filesystem::path& Path( ) const { return m_path; }
	void SetPath( const std::filesystem::path& path ) { m_path = path; }

	friend bool operator==( const SamplerOption& lhs, const SamplerOption& rhs )
	{
		return lhs.m_filter == rhs.m_filter
			&& lhs.m_addressU == rhs.m_addressU
			&& lhs.m_addressV == rhs.m_addressV
			&& lhs.m_addressW == rhs.m_addressW
			&& lhs.m_mipLODBias == rhs.m_mipLODBias
			&& lhs.m_comparisonFunc == rhs.m_comparisonFunc;
	}

	unsigned int m_filter = TEXTURE_FILTER::MIN_LINEAR | TEXTURE_FILTER::MAG_LINEAR | TEXTURE_FILTER::MIP_LINEAR;
	TEXTURE_ADDRESS_MODE m_addressU = TEXTURE_ADDRESS_MODE::CLAMP;
	TEXTURE_ADDRESS_MODE m_addressV = TEXTURE_ADDRESS_MODE::CLAMP;
	TEXTURE_ADDRESS_MODE m_addressW = TEXTURE_ADDRESS_MODE::CLAMP;
	float m_mipLODBias = 0.f;
	COMPARISON_FUNC m_comparisonFunc = COMPARISON_FUNC::NEVER;

protected:
	RENDERCORE_DLL virtual void PostLoadImpl( ) override;

private:
	std::filesystem::path m_path;
};

class RenderOption : public AsyncLoadableAsset
{
	DECLARE_ASSET( RENDERCORE, RenderOption );
public:
	RENDERCORE_DLL virtual void Serialize( Archive& ar ) override;

	friend bool operator==( const RenderOption& lhs, const RenderOption& rhs )
	{
		bool ret = true;

		ret &= ( lhs.m_vertexShader == rhs.m_vertexShader );
		ret &= ( lhs.m_pixelShader == rhs.m_pixelShader );
		ret &= ( lhs.m_blendOption == rhs.m_blendOption );
		ret &= ( lhs.m_depthStencilOption == rhs.m_depthStencilOption );
		ret &= ( lhs.m_rasterizerOption == rhs.m_rasterizerOption );

		for ( int i = 0; i < std::extent_v<decltype( lhs.m_samplerOption )>; ++i )
		{
			for ( int j = 0; j < std::extent_v<decltype( lhs.m_samplerOption ), 1>; ++j )
			{
				ret &= ( lhs.m_samplerOption[i][j] == rhs.m_samplerOption[i][j] );
			}
		}

		return ret;
	}

	std::shared_ptr<VertexShader> m_vertexShader = nullptr;
	std::shared_ptr<PixelShader> m_pixelShader = nullptr;
	std::shared_ptr<BlendOption> m_blendOption = nullptr;
	std::shared_ptr<DepthStencilOption> m_depthStencilOption = nullptr;
	std::shared_ptr<RasterizerOption> m_rasterizerOption = nullptr;
	static constexpr int SAMPLER_SLOT_COUNT = 16;
	std::shared_ptr<SamplerOption> m_samplerOption[MAX_SHADER_TYPE<int>][SAMPLER_SLOT_COUNT] = {};

protected:
	RENDERCORE_DLL virtual void PostLoadImpl( ) override;
};
