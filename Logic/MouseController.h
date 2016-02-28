#pragma once

#include <d3dX9math.h>
#include <vector>

enum class MOUSE_INPUT_TYPE
{
	L_DOWN = 0,
	L_UP,
	R_DOWN,
	R_UP,
	M_DOWN,
	M_UP,
	MOVE,
	WHEEL_MOVE,
	UNKNOWN,
};

struct MOUSE_INPUT_INFO
{
	int m_x;
	int m_y;
	int m_zDelta;
	MOUSE_INPUT_TYPE m_type;
};

class CWinProcMouseInputTranslator
{
public:
	MOUSE_INPUT_INFO TranslateInput( const int message, const WPARAM wParam, const LPARAM lParam );
};

class IListener;

class CMouseController
{
public:
	void SetLastDownPos( const D3DXVECTOR3& mousePos ) { m_lastDownPos = mousePos; }
	const D3DXVECTOR3& GetLastDownPos( ) { return m_lastDownPos; }

	void ProcessInput( const MOUSE_INPUT_INFO& input );

	void AddListener( IListener* listener );

private:
	D3DXVECTOR3 m_lastDownPos;
	std::vector<IListener*> m_listeners;
};

