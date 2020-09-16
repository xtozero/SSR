#pragma once

#include "GraphicsApiResource.h"

#include <string>

namespace JSON
{
	class Value;
}

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
};

struct BlendOption
{
	void LoadFromScript( const JSON::Value& blendOption );

	bool m_alphaToConverageEnable;
	bool m_independentBlendEnable;
	RenderTargetBlendOption m_renderTarget[8];
};

struct DepthOption
{
	bool m_enable = true;
	bool m_writeDepth = true;
	COMPARISON_FUNC m_depthFunc = COMPARISON_FUNC::LESS;
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
};

struct DepthStencilOption
{
	void LoadFromScript( const JSON::Value& depthStencilOption );

	DepthOption m_depth;
	StencilOption m_stencil;
};

struct RasterizerOption
{
	void LoadFromScript( const JSON::Value& rasterizerOption );

	bool m_isWireframe = false;
	CULL_MODE m_cullMode = CULL_MODE::BACK;
	bool m_counterClockwise = false;
	int m_depthBias = 0;
	bool m_depthClipEnable = true;
	bool m_scissorEnable = false;
	bool m_multisampleEnalbe = false;
	bool m_antialiasedLineEnable = false;
};

struct SamplerOption
{
	void LoadFromScript( const JSON::Value& samplerOption );

	unsigned int m_filter = TEXTURE_FILTER::MIN_LINEAR | TEXTURE_FILTER::MAG_LINEAR | TEXTURE_FILTER::MIP_LINEAR;
	TEXTURE_ADDRESS_MODE m_addressU = TEXTURE_ADDRESS_MODE::CLAMP;
	TEXTURE_ADDRESS_MODE m_addressV = TEXTURE_ADDRESS_MODE::CLAMP;
	TEXTURE_ADDRESS_MODE m_addressW = TEXTURE_ADDRESS_MODE::CLAMP;
	float m_mipLODBias = 0.f;
	COMPARISON_FUNC m_comparisonFunc = COMPARISON_FUNC::NEVER;
};

struct RenderOption
{
	void LoadFromScript( const JSON::Value& renderOption, 
						const JSON::Value& blendOptionAsset,
						const JSON::Value& depthStencilOptionAsset,
						const JSON::Value& rasterizerOptionAsset,
						const JSON::Value& samplerOptionAsset );

	std::string m_shader[MAX_SHADER_TYPE<int>];
	BlendOption m_blendOption;
	DepthStencilOption m_depthStencilOption;
	RasterizerOption m_rasterizerOption;
	SamplerOption m_samplerOption[MAX_SHADER_TYPE<int>];
};
