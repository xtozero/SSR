#include "stdafx.h"
#include "CommonRenderResource.h"

#include "GlobalShaders.h"

namespace rendercore
{
	REGISTER_GLOBAL_SHADER( FullScreenQuadVS, "./Assets/Shaders/Common/VS_FullScreenQuad.asset" );

	FullScreenQuadVS::FullScreenQuadVS()
	{
		m_shader = static_cast<VertexShader*>( GetGlobalShader<FullScreenQuadVS>()->CompileShader( {} ) );
	}
}
