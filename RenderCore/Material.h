#pragma once

#include "CommonRenderer/IMaterial.h"

#include <wrl/client.h>

class IShader;
class IBuffer;
class IRenderState;

class Material : public IMaterial
{
protected:
	IShader* m_pShaders[SHADER_TYPE::MAX_SHADER];
	IRenderState* m_pRasterizerState;
	IRenderState* m_pSamplerState[SHADER_TYPE::MAX_SHADER];
	IRenderState* m_pDepthStencilState;
	IRenderState* m_pBlendState;

public:
	virtual void Init( IRenderer& renderer ) override;
	virtual void SetShader( ) override;
	virtual void SetTexture( UINT shaderType, UINT slot, const IRenderResource* pTexture ) override;
	
	Material( );
};