#include "stdafx.h"
#include "Surface.h"

void Surface::SetAmbient( const CXMFLOAT4& ambient )
{
	m_trait.m_ambient = ambient;
}

void Surface::SetDiffuse( const CXMFLOAT4& diffuse )
{
	m_trait.m_diffuse = diffuse;
}

void Surface::SetSpecular( const CXMFLOAT4& specular )
{
	m_trait.m_specular = specular;
}

void Surface::SetRoughness( const float roughness )
{
	m_trait.m_roughness = roughness;
}

void Surface::SetSpeculaPower( const float speculaPower )
{
	m_trait.m_specularPower = speculaPower;
}

void Surface::SetTextureName( const String& textureName )
{
	m_textureName = textureName;
}