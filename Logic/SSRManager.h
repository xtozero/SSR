#pragma once

#include <memory>
#include <list>

#include "ScreenBlurManager.h"

class CGameLogic;
class CGameObject;
class IBuffer;
class IMaterial;
class IMesh;
class IRenderResource;

class CSSRManager
{
public:
	bool Init( CGameLogic& gameLogic );
	void Process( CGameLogic& gameLogic, const std::list<CGameObject*>& reflectableList ) const;
	void AppSizeChanged( CGameLogic& gameLogic );

private:
	bool CreateAppSizeDependentResource( CGameLogic& gameLogic );

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

