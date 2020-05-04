#pragma once

#include "GraphicsApiResource.h"

class IRenderer;

class CMaterial
{
protected:
	RE_HANDLE m_hShaders[MAX_SHADER_TYPE<int>];
	RE_HANDLE m_hRasterizerState;
	RE_HANDLE m_hSamplerState[MAX_SHADER_TYPE<int>];
	RE_HANDLE m_hDepthStencilState;
	RE_HANDLE m_hBlendState;

public:
	void Bind( IRenderer& renderer ) const;
};