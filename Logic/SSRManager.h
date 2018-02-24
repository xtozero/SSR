#pragma once

#include "INotifyGraphicsDevice.h"
#include "ScreenBlurManager.h"

#include "../RenderCore/CommonRenderer/Resource.h"

#include <memory>
#include <list>

class CGameLogic;
class CGameObject;
class IMaterial;
class IMesh;
class IRenderer;

class CSSRManager : public IGraphicsDeviceNotify
{
public:
	virtual void OnDeviceRestore( CGameLogic& gameLogic ) override;

	bool Init( CGameLogic& gameLogic );
	void Process( CGameLogic& gameLogic, const std::list<CGameObject*>& reflectableList ) const;
	void AppSizeChanged( CGameLogic& gameLogic );

private:
	bool CreateAppSizeDependentResource( IRenderer& renderer );
	bool CreateDeviceDependendResource( CGameLogic& gameLogic );

	IMesh* m_pScreenRect = nullptr;
	IMaterial* m_pSsrMaterial = nullptr;
	IMaterial* m_pSsrBlendMaterial = nullptr;
	RE_HANDLE m_ssrTexture = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_ssrRt = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_ssrSrv = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_defaultRt = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_defaultDS = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_defaultSrv = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_depthSrv = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_ssrConstantBuffer = RE_HANDLE_TYPE::INVALID_HANDLE;

	ScreenBlurManager m_blur;
};

