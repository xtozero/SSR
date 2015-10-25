#include "stdafx.h"

#include "TutorialMaterial.h"

void TutorialMaterial::Init( )
{
	m_pShaders[VS] = g_pRenderer->SearchShaderByName( _T( "vsTutorial" ) );

	m_pShaders[PS] = g_pRenderer->SearchShaderByName( _T( "psTutorial" ) );
}

TutorialMaterial::TutorialMaterial( )
{
	
}


TutorialMaterial::~TutorialMaterial( )
{
}
