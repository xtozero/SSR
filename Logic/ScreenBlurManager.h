#pragma once

#include "INotifyGraphicsDevice.h"
#include "../RenderCore/CommonRenderer/Resource.h"

#include <memory>

class CGameLogic;
class IMesh;
class IModelBuilder;
class IMaterial;
class IRenderResource;

class ScreenBlurManager : public IGraphicsDeviceNotify
{
public:
	virtual void OnDeviceRestore( CGameLogic& gameLogic ) override;

	bool Init( CGameLogic& gameLogic );
	void Process( CGameLogic& gameLogic, RE_HANDLE destSRV, RE_HANDLE destRT ) const;
	void AppSizeChanged( CGameLogic& gameLogic );

private:
	bool CreateDeviceDependentResource( CGameLogic& gameLogic );
	bool CreateAppSizeDependentResource( CGameLogic& gameLogic );

	IMesh* m_pScreenRect = nullptr;
	IMaterial* m_pBlurMaterial[2] = { nullptr, nullptr };
	RE_HANDLE m_blurTexture = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_blurRt = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_blurSrv = RE_HANDLE_TYPE::INVALID_HANDLE;
};

