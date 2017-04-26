#pragma once

#include <vector>

#include "../shared/CDirectXMath.h"

class IListener;
struct UserInput;

class CUserInputBroadCaster
{
public:
	void SetLastDownPos( const CXMFLOAT3& mousePos ) { m_lastDownPos = mousePos; }
	const CXMFLOAT3& GetLastDownPos( ) { return m_lastDownPos; }

	void ProcessInput( const UserInput& input );

	void AddListener( IListener* listener );

private:
	CXMFLOAT3 m_lastDownPos;
	std::vector<IListener*> m_listeners;
};

