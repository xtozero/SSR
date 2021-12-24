#include "stdafx.h"
#include "CommonRenderResource.h"

#include "GlobalShaders.h"

REGISTER_GLOBAL_SHADER( FullScreenQuadVS, "./Assets/Shaders/VS_FullScreenQuad.asset" );

FullScreenQuadVS::FullScreenQuadVS( )
{
	m_shader = static_cast<VertexShader*>( GetGlobalShader<FullScreenQuadVS>( ) );
}
