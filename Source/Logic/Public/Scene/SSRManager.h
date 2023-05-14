#pragma once

#include "INotifyGraphicsDevice.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"
//#include "Render/Resource.h"
#include "ScreenBlurManager.h"
#include "SizedTypes.h"

#include <memory>
#include <list>

class BaseMesh;
class CGameLogic;
class CGameObject;
class IRenderer;

class CSSRManager : public IGraphicsDeviceNotify
{
public:
	virtual void OnDeviceRestore( CGameLogic& gameLogic ) override;

	bool Init( CGameLogic& gameLogic );
	void PreProcess( CGameLogic& gameLogic, const std::list<CGameObject*>* renderableList ) const;
	void Process( CGameLogic& gameLogic, const std::list<CGameObject*>* renderableList ) const;
	void AppSizeChanged( CGameLogic& gameLogic );

private:
	bool CreateAppSizeDependentResource( IRenderer& renderer );
	bool CreateDeviceDependendResource( CGameLogic& gameLogic );

	struct SSRProperties
	{
		float depthbias;
		float rayStepScale;
		float maxThickness;
		float maxRayLength;
		int32 maxRayStep;
	};

	struct SSRConstantBuffer
	{
		Matrix projectionMatrix;
		SSRProperties properties;
		Vector pedding;
	};

	BaseMesh* m_pScreenRect = nullptr;
	//Material m_backfaceDepthMaterial = INVALID_MATERIAL;
	//Material m_ssrMaterial = INVALID_MATERIAL;
	//Material m_ssrBlendMaterial = INVALID_MATERIAL;
	//RE_HANDLE m_ssrTexture = RE_HANDLE::InValidHandle( );
	//RE_HANDLE m_ssrRt = RE_HANDLE::InValidHandle( );
	//RE_HANDLE m_ssrSrv = RE_HANDLE::InValidHandle( );
	//RE_HANDLE m_defaultRt = RE_HANDLE::InValidHandle( );
	//RE_HANDLE m_defaultDS = RE_HANDLE::InValidHandle( );
	//RE_HANDLE m_defaultSrv = RE_HANDLE::InValidHandle( );
	//RE_HANDLE m_depthSrv = RE_HANDLE::InValidHandle( );
	//RE_HANDLE m_ssrConstantBuffer = RE_HANDLE::InValidHandle( );
	//RE_HANDLE m_backfaceDepthTexture = RE_HANDLE::InValidHandle( );
	//RE_HANDLE m_backfaceDepthRt = RE_HANDLE::InValidHandle( );
	//RE_HANDLE m_backfaceDepthSrv = RE_HANDLE::InValidHandle( );

	ScreenBlurManager m_blur;
	mutable SSRProperties m_properties = { 
		.depthbias = 0.01f, 
		.rayStepScale = 3.f, 
		.maxThickness = 1.8f, 
		.maxRayLength = 100.f, 
		.maxRayStep = 100
	};

	mutable bool m_isEnabled = true;
};

