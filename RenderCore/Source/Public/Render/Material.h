#pragma once

#include "Resource.h"

class IRenderer;

class CMaterial
{
protected:
	RE_HANDLE m_hShaders[SHADER_TYPE::MAX_SHADER];
	RE_HANDLE m_hRasterizerState = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_hSamplerState[SHADER_TYPE::MAX_SHADER];
	RE_HANDLE m_hDepthStencilState = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_hBlendState = RE_HANDLE_TYPE::INVALID_HANDLE;

public:
	void Bind( IRenderer& renderer ) const;
	
	bool SetShader( IRenderer& renderer, const SHADER_TYPE type, const String& shaderName );
	bool SetSamplerState( IRenderer& renderer, const SHADER_TYPE type, const String& samplerName );
	bool SetDepthStencilState( IRenderer& renderer, const String& dsStateName );
	bool SetRasterizerState( IRenderer& renderer, const String& rsStateName );
	bool SetBlendState( IRenderer& renderer, const String& blendStateName );

	CMaterial( );
};