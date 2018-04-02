#pragma once

#include "INotifyGraphicsDevice.h"
#include "Render/Resource.h"

#include <memory>

class CGameLogic;
class IMesh;
class IModelBuilder;
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
	Material m_blurMaterial[2] = { INVALID_MATERIAL, INVALID_MATERIAL };
	RE_HANDLE m_blurTexture = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_blurRt = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_blurSrv = RE_HANDLE_TYPE::INVALID_HANDLE;
};

