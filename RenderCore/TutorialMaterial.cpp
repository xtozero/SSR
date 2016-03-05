#include "stdafx.h"
#include "IRenderer.h"

#include "TutorialMaterial.h"

extern IRenderer* g_pRenderer;

void TutorialMaterial::Init( )
{
	m_pShaders[SHADER_TYPE::VS] = g_pRenderer->SearchShaderByName( _T( "vsTutorial" ) );

	m_pShaders[SHADER_TYPE::PS] = g_pRenderer->SearchShaderByName( _T( "psTutorial" ) );
}