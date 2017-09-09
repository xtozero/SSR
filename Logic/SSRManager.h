#pragma once

#include <memory>
#include <list>

#include "ScreenBlurManager.h"

class CGameObject;
class IBuffer;
class IDepthStencil;
class IMaterial;
class IMesh;
class IMeshBuilder;
class IRenderer;
class IRenderState;
class IRenderTarget;
class IShaderResource;

class CSSRManager
{
public:
	bool Init( IRenderer& renderer, IMeshBuilder& meshBuilder );
	void Process( IRenderer& renderer, const std::list<CGameObject*>& reflectableList ) const;

private:
	IMesh* m_pScreenRect = nullptr;
	IMaterial* m_pSsrMaterial = nullptr;
	IMaterial* m_pSsrBlendMaterial = nullptr;
	IRenderTarget* m_pSsrRt = nullptr;
	IRenderTarget* m_pDefaultRt = nullptr;
	IDepthStencil* m_pDefaultDS = nullptr;
	IShaderResource* m_pSsrSrv = nullptr;
	IShaderResource* m_pDefaultSrv = nullptr;
	IShaderResource* m_pDepthSrv = nullptr;
	IBuffer* m_ssrConstantBuffer = nullptr;

	ScreenBlurManager m_blur;
};

