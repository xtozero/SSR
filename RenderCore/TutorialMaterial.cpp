#include "stdafx.h"
#include "TutorialMaterial.h"

void TutorialMaterial::Init( )
{
	m_pShaders[SHADER_TYPE::VS] = g_pRenderer->SearchShaderByName( _T( "vsTutorial" ) );

	m_pShaders[SHADER_TYPE::PS] = g_pRenderer->SearchShaderByName( _T( "psTutorial" ) );
}

TutorialMaterial::TutorialMaterial( )
{
	
}


TutorialMaterial::~TutorialMaterial( )
{
}
