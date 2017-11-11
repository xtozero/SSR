#include "stdafx.h"
#include "CommonRenderer/IRenderer.h"
#include "WireFrame.h"

void WireFrame::Init( IRenderer& renderer )
{
	Material::Init( renderer );

	m_pShaders[SHADER_TYPE::VS] = renderer.FindGraphicsShaderByName( _T( "vsTutorial" ) );

	m_pShaders[SHADER_TYPE::PS] = renderer.FindGraphicsShaderByName( _T( "psTutorial" ) );

	m_pRasterizerState = renderer.CreateRenderState( _T( "wireFrame" ) );
}
