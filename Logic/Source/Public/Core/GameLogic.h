#pragma once

#include "common.h"
#include "GameObject/Camera.h"
#include "GameObject/LightManager.h"
#include "GameObject/PickingManager.h"
#include "ILogic.h"
#include "Model/ModelBuilder.h"
#include "Model/ModelManager.h"
#include "Model/Surface.h"
#include "Scene/ConstantBufferDefine.h"
#include "Scene/RenderView.h"
#include "Scene/SceneLoader.h"
#include "Scene/ShadowManager.h"
#include "Scene/SSRManager.h"
#include "Timer.h"
#include "UserInput/MouseController.h"

#include <list>
#include <memory>
#include <vector>

class CGameObject;
class IRenderer;

enum RENDERABLE_TYPE
{
	OPAQUE_RENDERABLE = 0,
	TRANSPARENT_RENDERABLE,
	REFLECT_RENDERABLE,
	RENDERABLE_TYPE_COUNT,
};

class CGameLogic : public ILogic
{
public:
	virtual bool Initialize( IPlatform& platform ) override;
	virtual void Update() override;
	virtual void Pause( ) override;
	virtual void Resume( ) override;
	virtual void HandleUserInput( const UserInput& input ) override;
	virtual void AppSizeChanged( IPlatform& platform ) override;

	IRenderer& GetRenderer( ) const { return *m_pRenderer; }
	CModelManager& GetModelManager( ) { return m_meshManager; }
	RE_HANDLE GetCommonConstantBuffer( int purpose ) { return m_commonConstantBuffer[purpose]; }
	CRenderView& GetView( ) { return m_view; }
	const std::pair<UINT, UINT>& GetAPPSize( ) { return m_appSize; }

private:
	void StartLogic ( void );
	void ProcessLogic ( void );
	void EndLogic ( void );

	bool LoadScene( const String& scene );

	void SceneBegin( void );
	void DrawScene( void );
	void SceneEnd( void );

	void BuildRenderableList( );
	void DrawOpaqueRenderable( );
	void DrawTransparentRenderable( );
	void DrawReflectRenderable( );

	void HandleDeviceLost( );
	bool CreateDeviceDependentResource( );

public:
	CGameLogic();
	~CGameLogic() = default;

private:
	HWND	m_wndHwnd;
	std::pair<UINT, UINT> m_appSize;

	CCamera m_mainCamera;
	CUserInputBroadCaster m_inputBroadCaster;
	CPickingManager m_pickingManager;
	CLightManager m_lightManager;
	CShadowManager m_shadowManager;
	CSSRManager m_ssrManager;
	CModelManager m_meshManager;
	std::unique_ptr<IRenderer> m_pRenderer;
	CRenderView m_view;
	std::vector<std::unique_ptr<CGameObject>> m_gameObjects;

	std::list<CGameObject*> m_renderableList[RENDERABLE_TYPE_COUNT];

	RE_HANDLE m_commonConstantBuffer[SHARED_CONSTANT_BUFFER::Count];
};
