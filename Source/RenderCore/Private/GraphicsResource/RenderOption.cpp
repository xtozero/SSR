#include "stdafx.h"
#include "RenderOption.h"

#include "FileSystem.h"
#include "Json/json.hpp"
#include "Shader.h"

#include <cassert>

namespace fs = std::filesystem;

namespace rendercore
{
	Archive& operator<<( Archive& ar, RenderTargetBlendOption& option )
	{
		ar << option.m_blendEnable;
		ar << option.m_srcBlend;
		ar << option.m_destBlend;
		ar << option.m_blendOp;
		ar << option.m_srcBlendAlpha;
		ar << option.m_destBlendAlpha;
		ar << option.m_blendOpAlpha;
		ar << option.m_renderTargetWriteMask;

		return ar;
	}

	REGISTER_ASSET( BlendOption );
	void BlendOption::PostLoadImpl()
	{
	}

	Archive& operator<<( Archive& ar, DepthOption depthOption )
	{
		ar << depthOption.m_enable;
		ar << depthOption.m_writeDepth;
		ar << depthOption.m_depthFunc;

		return ar;
	}

	Archive& operator<<( Archive& ar, StencilOption stencilOption )
	{
		ar << stencilOption.m_enable;
		ar << stencilOption.m_readMask;
		ar << stencilOption.m_writeMask;

		ar << stencilOption.m_frontFace.m_failOp;
		ar << stencilOption.m_frontFace.m_depthFailOp;
		ar << stencilOption.m_frontFace.m_passOp;
		ar << stencilOption.m_frontFace.m_func;

		ar << stencilOption.m_backFace.m_failOp;
		ar << stencilOption.m_backFace.m_depthFailOp;
		ar << stencilOption.m_backFace.m_passOp;
		ar << stencilOption.m_backFace.m_func;

		ar << stencilOption.m_ref;

		return ar;
	}

	REGISTER_ASSET( DepthStencilOption );
	void DepthStencilOption::PostLoadImpl()
	{
	}

	REGISTER_ASSET( RasterizerOption );
	void RasterizerOption::PostLoadImpl()
	{
	}

	REGISTER_ASSET( SamplerOption );
	void SamplerOption::PostLoadImpl()
	{
	}

	REGISTER_ASSET( RenderOption );
	void RenderOption::PostLoadImpl()
	{
	}
}
