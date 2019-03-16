#pragma once

#include "Math/CXMFloat.h"

#include <vector>

class IListener;
class CGameLogic;
struct UserInput;

class CUserInputBroadCaster
{
public:
	void SetLastDownPos( const CXMFLOAT3& mousePos ) { m_lastDownPos = mousePos; }
	const CXMFLOAT3& GetLastDownPos( ) { return m_lastDownPos; }

	void ProcessInput( const UserInput& input, CGameLogic& gameLogic );

	void AddListener( IListener* listener );

private:
	CXMFLOAT3 m_lastDownPos;
	std::vector<IListener*> m_listeners;
};

