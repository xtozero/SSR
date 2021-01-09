#pragma once

#include "AssetLoader/AssetFactory.h"
#include "Core/IAsyncLoadableAsset.h"
#include "GraphicsApiResource.h"
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

class BlendOption : public AsyncLoadableAsset
{
	DECLARE_ASSET( BlendOption );
public:
	LOGIC_DLL virtual void Serialize( Archive& ar ) override;

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

private:
	std::filesystem::path m_path;
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
	unsigned int m_ref;

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

class DepthStencilOption : public AsyncLoadableAsset
{
	DECLARE_ASSET( DepthStencilOption );
public:
	LOGIC_DLL virtual void Serialize( Archive& ar ) override;

	const std::filesystem::path& Path( ) const { return m_path; }
	void SetPath( const std::filesystem::path& path ) { m_path = path; }

	friend bool operator==( const DepthStencilOption& lhs, const DepthStencilOption& rhs )
	{
		return lhs.m_depth == rhs.m_depth
			&& lhs.m_stencil == rhs.m_stencil;
	}

	DepthOption m_depth;
	StencilOption m_stencil;

private:
	std::filesystem::path m_path;
};

class RasterizerOption : public AsyncLoadableAsset
{
	DECLARE_ASSET( RasterizerOption );
public:
	LOGIC_DLL virtual void Serialize( Archive& ar ) override;

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

private:
	std::filesystem::path m_path;
};

class SamplerOption : public AsyncLoadableAsset
{
	DECLARE_ASSET( SamplerOption );
public:
	LOGIC_DLL virtual void Serialize( Archive& ar ) override;

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

private:
	std::filesystem::path m_path;
};

class RenderOption : public AsyncLoadableAsset
{
	DECLARE_ASSET( RenderOption );
public:
	LOGIC_DLL virtual void Serialize( Archive& ar ) override;

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
			for ( int j = 0; j < std::extent_v<decltype( lhs.m_samplerOption), 1>; ++j )
			{
				ret &= ( lhs.m_samplerOption[i][j] == rhs.m_samplerOption[i][j] );
			}
		}

		for ( int i = 0; i < std::extent_v<decltype( lhs.m_shaderPath )>; ++i )
		{
			ret &= ( lhs.m_shaderPath[i] == rhs.m_shaderPath[i] );
		}

		for ( int i = 0; i < std::extent_v<decltype( lhs.m_samplerOptionPath )>; ++i )
		{
			ret &= ( lhs.m_samplerOptionPath[i] == rhs.m_samplerOptionPath[i] );
		}

		return ret;
	}

	RefHandle<VertexShader> m_vertexShader = nullptr;
	RefHandle<PixelShader> m_pixelShader = nullptr;
	BlendOption* m_blendOption = nullptr;
	DepthStencilOption* m_depthStencilOption = nullptr;
	RasterizerOption* m_rasterizerOption = nullptr;
	SamplerOption* m_samplerOption[MAX_SHADER_TYPE<int>][16] = {};

	std::filesystem::path m_shaderPath[MAX_SHADER_TYPE<int>];
	std::filesystem::path m_samplerOptionPath[MAX_SHADER_TYPE<int>];
};
