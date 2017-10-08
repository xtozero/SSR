#pragma once

#include <memory>
#include <list>

#include "ScreenBlurManager.h"

class CGameObject;
class IBuffer;
class IMaterial;
class IMesh;
class IMeshBuilder;
class IRenderer;
class IRenderResource;
class IRenderState;

class CSSRManager
{
public:
	bool Init( IRenderer& renderer, IMeshBuilder& meshBuilder );
	void Process( IRenderer& renderer, const std::list<CGameObject*>& reflectableList ) const;

private:
	IMesh* m_pScreenRect = nullptr;
	IMaterial* m_pSsrMaterial = nullptr;
	IMaterial* m_pSsrBlendMaterial = nullptr;
	IRenderResource* m_pSsrRt = nullptr;
	IRenderResource* m_pDefaultRt = nullptr;
	IRenderResource* m_pDefaultDS = nullptr;
	IRenderResource* m_pSsrSrv = nullptr;
	IRenderResource* m_pDefaultSrv = nullptr;
	IRenderResource* m_pDepthSrv = nullptr;
	IBuffer* m_ssrConstantBuffer = nullptr;

	ScreenBlurManager m_blur;
};

