#include "stdafx.h"
#include "CommonRenderer/IRenderer.h"

#include "TutorialMaterial.h"

void TutorialMaterial::Init( IRenderer& renderer )
{
	Material::Init( renderer );

	m_pShaders[SHADER_TYPE::VS] = renderer.FindGraphicsShaderByName( _T( "vsTutorial" ) );

	m_pShaders[SHADER_TYPE::PS] = renderer.FindGraphicsShaderByName( _T( "psTutorial" ) );
}