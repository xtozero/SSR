#pragma once

class CGameLogic;
struct UserInput;

class IListener
{
public:
	virtual void ProcessInput( const UserInput&, CGameLogic& gameLogic ) = 0;

	virtual ~IListener( ) = default;

protected:
	IListener( ) = default;
};