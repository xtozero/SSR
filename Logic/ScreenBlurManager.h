#pragma once
#include <memory>

class CGameLogic;
class IMesh;
class IModelBuilder;
class IMaterial;
class IRenderResource;

class ScreenBlurManager
{
public:
	bool Init( CGameLogic& gameLogic, IModelBuilder& meshBuilder );
	void Process( CGameLogic& gameLogic, IRenderResource& destSRV, IRenderResource& destRT ) const;
	void AppSizeChanged( CGameLogic& gameLogic );

private:
	bool CreateAppSizeDependentResource( CGameLogic& gameLogic );

	IMesh* m_pScreenRect = nullptr;
	IMaterial* m_pBlurMaterial[2] = { nullptr, nullptr };
	IRenderResource* m_pBlurRt = nullptr;
	IRenderResource* m_pBlurSrv = nullptr;
};

