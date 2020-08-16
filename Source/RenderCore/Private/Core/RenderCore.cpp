#include "stdafx.h"
#include "Renderer/IRenderCore.h"

#include "common.h"
#include "IAga.h"

class RenderCore : public IRenderCore
{
public:
	virtual bool BootUp( HWND hWnd, UINT nWndWidth, UINT nWndHeight ) override;
	virtual void HandleDeviceLost( HWND hWnd, UINT nWndWidth, UINT nWndHeight ) override;
	virtual void AppSizeChanged( UINT nWndWidth, UINT nWndHeight ) override;

	~RenderCore( );

private:
	void Shutdown( );

	HMODULE m_hAga;
	IAga* m_AbstractGraphicsAPI = nullptr;
};

Owner<IRenderCore*> CreateRenderCore( )
{
	return new RenderCore( );
}

void DestoryRenderCore( Owner<IRenderCore*> pRenderCore )
{
	delete pRenderCore;
}

bool RenderCore::BootUp( HWND hWnd, UINT nWndWidth, UINT nWndHeight )
{
	m_hAga = LoadModule( "./Binaries/Aga.dll" );
	if ( m_hAga == nullptr )
	{
		return false;
	}

	m_AbstractGraphicsAPI = GetInterface<IAga>();
	if ( m_AbstractGraphicsAPI == nullptr )
	{
		return false;
	}

	if ( m_AbstractGraphicsAPI->BootUp( hWnd, nWndWidth, nWndHeight ) == false )
	{
		return false;
	}

	return true;
}

void RenderCore::HandleDeviceLost( HWND hWnd, UINT nWndWidth, UINT nWndHeight )
{
}

void RenderCore::AppSizeChanged( UINT nWndWidth, UINT nWndHeight )
{
	m_AbstractGraphicsAPI->AppSizeChanged( nWndWidth, nWndHeight );
}

RenderCore::~RenderCore( )
{
	Shutdown( );
}

void RenderCore::Shutdown( )
{
	ShutdownModule( m_hAga );
}
