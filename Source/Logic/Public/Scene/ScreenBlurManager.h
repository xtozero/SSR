#if 0
#pragma once

#include <memory>

namespace rendercore
{
	class BaseMesh;
}

namespace logic
{
	class CGameLogic;
	//class IModelBuilder;
	class IRenderResource;

	class ScreenBlurManager
	{
	public:
		bool Init( CGameLogic& gameLogic );
		//void Process( CGameLogic& gameLogic, RE_HANDLE destSRV, RE_HANDLE destRT ) const;
		void AppSizeChanged( CGameLogic& gameLogic );

	private:
		bool CreateDeviceDependentResource( CGameLogic& gameLogic );
		bool CreateAppSizeDependentResource( CGameLogic& gameLogic );

		//BaseMesh* m_pScreenRect = nullptr;
		//Material m_blurMaterial[2] = { INVALID_MATERIAL, INVALID_MATERIAL };
		//RE_HANDLE m_blurTexture = RE_HANDLE::InValidHandle( );
		//RE_HANDLE m_blurRt = RE_HANDLE::InValidHandle( );
		//RE_HANDLE m_blurSrv = RE_HANDLE::InValidHandle( );
	};
}
#endif
