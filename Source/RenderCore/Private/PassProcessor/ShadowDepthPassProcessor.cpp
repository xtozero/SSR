#include "stdafx.h"
#include "ShadowDepthPassProcessor.h"

#include "AbstractGraphicsInterface.h"
#include "GlobalShaders.h"
#include "PrimitiveProxy.h"
#include "Scene/PrimitiveSceneInfo.h"
#include "VertexCollection.h"

namespace
{
	IPassProcessor* CreateShadowDepthPassProcessor( )
	{
		return new ShadowDepthPassProcessor( );
	}
}

class ShadowDepthVS
{
public:
	ShadowDepthVS( );
	VertexShader* Shader( ) { return m_shader; }

private:
	VertexShader* m_shader = nullptr;
};

class ShadowDepthGS
{
public:
	ShadowDepthGS( );
	GeometryShader* Shader( ) { return m_shader; }

private:
	GeometryShader* m_shader = nullptr;
};

class ShadowDepthPS
{
public:
	ShadowDepthPS( );
	PixelShader* Shader( ) { return m_shader; }

private:
	PixelShader* m_shader = nullptr;
};

REGISTER_GLOBAL_SHADER( ShadowDepthVS, "./Assets/Shaders/VS_CascadedShadowmap.asset" );
REGISTER_GLOBAL_SHADER( ShadowDepthGS, "./Assets/Shaders/GS_CascadedShadowmap.asset" );
REGISTER_GLOBAL_SHADER( ShadowDepthPS, "./Assets/Shaders/PS_CascadedShadowmap.asset" );

ShadowDepthVS::ShadowDepthVS( )
{
	m_shader = static_cast<VertexShader*>( GetGlobalShader<ShadowDepthVS>( ) );
}

ShadowDepthGS::ShadowDepthGS( )
{
	m_shader = static_cast<GeometryShader*>( GetGlobalShader<ShadowDepthGS>( ) );
}

ShadowDepthPS::ShadowDepthPS( )
{
	m_shader = static_cast<PixelShader*>( GetGlobalShader<ShadowDepthPS>( ) );
}

std::optional<DrawSnapshot> ShadowDepthPassProcessor::Process( const PrimitiveSubMesh& subMesh )
{
	assert( IsInRenderThread( ) );

	PassShader passShader{
		ShadowDepthVS( ).Shader( ),
		ShadowDepthGS( ).Shader( ),
		ShadowDepthPS( ).Shader( )
	};

	PassRenderOption passRenderOption;
	if ( const RenderOption* option = subMesh.m_renderOption )
	{
		if ( option->m_blendOption )
		{
			passRenderOption.m_blendOption = &( *option->m_blendOption );
		}
	}

	DepthStencilOption shadowDepthPassDepthStencilOption;
	passRenderOption.m_depthStencilOption = &shadowDepthPassDepthStencilOption;

	RasterizerOption shadowDepthPassRasterizerOption;
	passRenderOption.m_rasterizerOption = &shadowDepthPassRasterizerOption;

	return BuildDrawSnapshot( subMesh, passShader, passRenderOption, VertexStreamLayoutType::PositionOnly );
}

PassProcessorRegister RegisterShadowDepthPass( RenderPass::CSMShadowDepth, &CreateShadowDepthPassProcessor );