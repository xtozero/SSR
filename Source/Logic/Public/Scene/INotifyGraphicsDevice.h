#pragma once

class CGameLogic;

class IGraphicsDeviceNotify
{
public:
	virtual void OnDeviceRestore( CGameLogic& gameLogic ) = 0;

	virtual ~IGraphicsDeviceNotify( ) = default;
};