#include "stdafx.h"
#include "RenderOption.h"

#include "FileSystem/EngineFileSystem.h"
#include "Json/json.hpp"
#include "Shader.h"

#include <cassert>

namespace fs = std::filesystem;

REGISTER_ASSET( BlendOption );
void BlendOption::Serialize( Archive& ar )
{
	if ( ar.IsWriteMode( ) )
	{
		ar << ID;
	}

	ar << m_alphaToConverageEnable;
	ar << m_independentBlendEnable;
	for ( const RenderTargetBlendOption& rt : m_renderTarget )
	{
		ar << rt.m_blendEnable;
		ar << rt.m_srcBlend;
		ar << rt.m_destBlend;
		ar << rt.m_blendOp;
		ar << rt.m_srcBlendAlpha;
		ar << rt.m_destBlendAlpha;
		ar << rt.m_blendOpAlpha;
		ar << rt.m_renderTargetWriteMask;
	}
}

void BlendOption::PostLoadImpl( )
{
}

REGISTER_ASSET( DepthStencilOption );
void DepthStencilOption::Serialize( Archive& ar )
{
	if ( ar.IsWriteMode( ) )
	{
		ar << ID;
	}

	ar << m_depth.m_enable;
	ar << m_depth.m_writeDepth;
	ar << m_depth.m_depthFunc;

	ar << m_stencil.m_enable;
	ar << m_stencil.m_readMask;
	ar << m_stencil.m_writeMask;

	ar << m_stencil.m_frontFace.m_failOp;
	ar << m_stencil.m_frontFace.m_depthFailOp;
	ar << m_stencil.m_frontFace.m_passOp;
	ar << m_stencil.m_frontFace.m_func;

	ar << m_stencil.m_backFace.m_failOp;
	ar << m_stencil.m_backFace.m_depthFailOp;
	ar << m_stencil.m_backFace.m_passOp;
	ar << m_stencil.m_backFace.m_func;

	ar << m_stencil.m_ref;
}

void DepthStencilOption::PostLoadImpl( )
{
}

REGISTER_ASSET( RasterizerOption );
void RasterizerOption::Serialize( Archive& ar )
{
	if ( ar.IsWriteMode( ) )
	{
		ar << ID;
	}

	ar << m_isWireframe;
	ar << m_cullMode;
	ar << m_counterClockwise;
	ar << m_depthBias;
	ar << m_depthClipEnable;
	ar << m_scissorEnable;
	ar << m_multisampleEnalbe;
	ar << m_antialiasedLineEnable;
}

void RasterizerOption::PostLoadImpl( )
{
}

REGISTER_ASSET( SamplerOption );
void SamplerOption::Serialize( Archive& ar )
{
	if ( ar.IsWriteMode( ) )
	{
		ar << ID;
	}

	ar << m_filter;
	ar << m_addressU;
	ar << m_addressV;
	ar << m_addressW;
	ar << m_mipLODBias;
	ar << m_comparisonFunc;
}

void SamplerOption::PostLoadImpl( )
{
}

REGISTER_ASSET( RenderOption );
void RenderOption::Serialize( Archive& ar )
{
	if ( ar.IsWriteMode( ) )
	{
		ar << ID;
	}

	ar << m_vertexShader;
	ar << m_pixelShader;

	ar << m_blendOption;
	ar << m_depthStencilOption;
	ar << m_rasterizerOption;

	for ( int i = 0; i < MAX_SHADER_TYPE<int>; ++i )
	{
		for ( int slot = 0; slot < SAMPLER_SLOT_COUNT; ++slot )
		{
			ar << m_samplerOption[i][slot];
		}
	}
}

void RenderOption::PostLoadImpl( )
{
}
