#include "stdafx.h"
#include "IRenderer.h"

#include "TutorialMaterial.h"

void TutorialMaterial::Init( IRenderer* pRenderer )
{
	Material::Init( pRenderer );

	if ( pRenderer )
	{
		m_pShaders[SHADER_TYPE::VS] = pRenderer->SearchShaderByName( _T( "vsTutorial" ) );

		m_pShaders[SHADER_TYPE::PS] = pRenderer->SearchShaderByName( _T( "psTutorial" ) );
	}
}