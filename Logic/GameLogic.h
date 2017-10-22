#pragma once

#include "Camera.h"
#include "common.h"
#include "ConstantBufferDefine.h"
#include "ILogic.h"
#include "LightManager.h"
#include "MouseController.h"
#include "PickingManager.h"
#include "SceneLoader.h"
#include "ShadowManager.h"
#include "SSRManager.h"
#include "Timer.h"

#include "Model/ModelBuilder.h"
#include "Model/ModelManager.h"
#include "Model/Surface.h"

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
	virtual void HandleUserInput( const UserInput& input );

	IRenderer& GetRenderer( ) const { return *m_pRenderer; }
	CModelManager& GetModelManager( ) { return m_meshManager; }
	IBuffer& GetCommonConstantBuffer( int purpose ) { return *m_commonConstantBuffer[purpose]; }

private:
	void StartLogic ( void );
	void ProcessLogic ( void );
	void EndLogic ( void );

	bool LoadScene( void );

	void SceneBegin( void ) const;
	void DrawScene( void );
	void SceneEnd( void ) const;

	void UpdateWorldMatrix( CGameObject* object ) const;

	void BuildRenderableList( );
	void DrawOpaqueRenderable( );
	void DrawTransparentRenderable( );
	void DrawReflectRenderable( );

public:
	CGameLogic ();
	~CGameLogic () = default;

private:
	HWND	m_wndHwnd;
	std::pair<UINT, UINT> m_wndSize;

	CCamera m_mainCamera;
	CUserInputBroadCaster m_inputBroadCaster;
	CPickingManager m_pickingManager;
	CLightManager m_lightManager;
	CShadowManager m_shadowManager;
	CSSRManager m_ssrManager;
	CModelManager m_meshManager;
	IRenderer* m_pRenderer;
	std::vector<std::unique_ptr<CGameObject>> m_gameObjects;

	std::list<CGameObject*> m_renderableList[RENDERABLE_TYPE_COUNT];

	IBuffer* m_commonConstantBuffer[COMMON_CONSTANT_BUFFER::Count] = { nullptr, };
};
