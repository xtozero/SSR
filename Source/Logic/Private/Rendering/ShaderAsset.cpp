#include "stdafx.h"
#include "Rendering/ShaderAsset.h"

REGISTER_ASSET( VertexShaderAsset );
//void VertexShaderAsset::LoadFromAsset( const char* asset, unsigned long assetSize )
//{
//	m_shader = VertexShader::Create( asset, static_cast<std::size_t>( assetSize ) );
//}

void VertexShaderAsset::Serialize( Archive& ar )
{
}

REGISTER_ASSET( PixelShaderAsset );
//void PixelShaderAsset::LoadFromAsset( const char* asset, unsigned long assetSize )
//{
//	m_shader = PixelShader::Create( asset, static_cast<std::size_t>( assetSize ) );
//}

void PixelShaderAsset::Serialize( Archive& ar )
{
}
