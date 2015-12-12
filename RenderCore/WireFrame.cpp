#include "stdafx.h"
#include "WireFrame.h"

void WireFrame::Init( )
{
	m_pShaders[VS] = g_pRenderer->SearchShaderByName( _T( "vsTutorial" ) );

	m_pShaders[PS] = g_pRenderer->SearchShaderByName( _T( "psTutorial" ) );

	m_pRenderState = g_pRenderer->CreateRenderState( true, false );
}

WireFrame::WireFrame( )
{
}


WireFrame::~WireFrame( )
{
}
