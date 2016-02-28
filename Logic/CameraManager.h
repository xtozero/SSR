#pragma once

class CCamera;

class CCameraManager
{
public:
	static CCameraManager* GetInstance( )
	{
		static CCameraManager instance;
		return &instance;
	}

	CCamera* GetCurrentCamera( ) const { return m_curCamera; }
	void SetCurrentCamera( CCamera* camera ) { m_curCamera = camera; }

	CCameraManager( );
private:
	CCamera* m_curCamera;
};

