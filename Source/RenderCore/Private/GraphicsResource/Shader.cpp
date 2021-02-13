#include "stdafx.h"
#include "Shader.h"

void ShaderBase::Serialize( Archive& ar )
{
	ar << m_byteCode;
}

//void VertexShaderAsset::LoadFromAsset( const char* asset, unsigned long assetSize )
//{
//	m_shader = VertexShader::Create( asset, static_cast<std::size_t>( assetSize ) );
//}


//void PixelShaderAsset::LoadFromAsset( const char* asset, unsigned long assetSize )
//{
//	m_shader = PixelShader::Create( asset, static_cast<std::size_t>( assetSize ) );
//}

REGISTER_ASSET( VertexShader );
void VertexShader::Serialize( Archive& ar )
{
	if ( ar.IsWriteMode( ) )
	{
		ar << ID;
	}

	ShaderBase::Serialize( ar );
}

void VertexShader::PostLoadImpl( )
{
}

REGISTER_ASSET( PixelShader );
void PixelShader::Serialize( Archive& ar )
{
	if ( ar.IsWriteMode( ) )
	{
		ar << ID;
	}

	ShaderBase::Serialize( ar );
}

void PixelShader::PostLoadImpl( )
{
}
