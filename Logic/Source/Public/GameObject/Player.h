#pragma once

#include "Camera.h"
#include "Core/IListener.h"
#include "Scene/INotifyGraphicsDevice.h"

class CPlayer : public IListener, public IGraphicsDeviceNotify
{
public:
	virtual void ProcessInput( const UserInput& input, CGameLogic& gameLogic ) override;
	virtual void OnDeviceRestore( CGameLogic& gameLogic ) override;

	CCamera& GetCamera( ) { return m_camera; }

private:
	void OnMouseLButton( const UserInput& input, CGameLogic& gameLogic );

	CCamera m_camera;
};
