#pragma once

#include "CommonRenderer/IMaterial.h"

#include <wrl/client.h>

class Material : public IMaterial
{
protected:
	RE_HANDLE m_hShaders[SHADER_TYPE::MAX_SHADER];
	RE_HANDLE m_hRasterizerState = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_hSamplerState[SHADER_TYPE::MAX_SHADER];
	RE_HANDLE m_hDepthStencilState = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_hBlendState = RE_HANDLE_TYPE::INVALID_HANDLE;

public:
	virtual void Init( IRenderer& ) override {};
	virtual void Bind( IRenderer& renderer ) override;
	
	Material( );
};