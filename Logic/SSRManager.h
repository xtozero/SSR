#pragma once

#include "INotifyGraphicsDevice.h"
#include "ScreenBlurManager.h"

#include <memory>
#include <list>

class CGameLogic;
class CGameObject;
class IBuffer;
class IMaterial;
class IMesh;
class IRenderer;
class IRenderResource;

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
	IRenderResource* m_pSsrRt = nullptr;
	IRenderResource* m_pDefaultRt = nullptr;
	IRenderResource* m_pDefaultDS = nullptr;
	IRenderResource* m_pSsrSrv = nullptr;
	IRenderResource* m_pDefaultSrv = nullptr;
	IRenderResource* m_pDepthSrv = nullptr;
	IBuffer* m_ssrConstantBuffer = nullptr;

	ScreenBlurManager m_blur;
};

