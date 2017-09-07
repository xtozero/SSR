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

	CSSRManager( );

private:
	std::shared_ptr<IMesh> m_pScreenRect;
	IMaterial* m_pSsrMaterial;
	IMaterial* m_pSsrBlendMaterial;
	IRenderTarget* m_pSsrRt;
	IRenderTarget* m_pDefaultRt;
	IDepthStencil* m_pDefaultDS;
	IShaderResource* m_pSsrSrv;
	IShaderResource* m_pDefaultSrv;
	IShaderResource* m_pDepthSrv;
	IBuffer* m_ssrConstantBuffer;

	ScreenBlurManager m_blur;
};

