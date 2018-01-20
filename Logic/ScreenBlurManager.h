#pragma once

#include "INotifyGraphicsDevice.h"

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
	void Process( CGameLogic& gameLogic, IRenderResource& destSRV, IRenderResource& destRT ) const;
	void AppSizeChanged( CGameLogic& gameLogic );

private:
	bool CreateDeviceDependentResource( CGameLogic& gameLogic );
	bool CreateAppSizeDependentResource( CGameLogic& gameLogic );

	IMesh* m_pScreenRect = nullptr;
	IMaterial* m_pBlurMaterial[2] = { nullptr, nullptr };
	IRenderResource* m_pBlurRt = nullptr;
	IRenderResource* m_pBlurSrv = nullptr;
};

