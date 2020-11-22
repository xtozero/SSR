#include "stdafx.h"
#include "Rendering/ShaderAsset.h"

void VertexShaderAsset::LoadFromAsset( const char* asset, unsigned long assetSize )
{
	m_shader = VertexShader::Create( asset, static_cast<std::size_t>( assetSize ) );
}

void PixelShaderAsset::LoadFromAsset( const char* asset, unsigned long assetSize )
{
	m_shader = PixelShader::Create( asset, static_cast<std::size_t>( assetSize ) );
}
