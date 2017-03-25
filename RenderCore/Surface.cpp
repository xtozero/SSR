#include "stdafx.h"
#include "Surface.h"

void CSurface::SetAmbient( const CXMFLOAT4& ambient )
{
	m_trait.m_ambient = ambient;
}

void CSurface::SetDiffuse( const CXMFLOAT4& diffuse )
{
	m_trait.m_diffuse = diffuse;
}

void CSurface::SetSpecular( const CXMFLOAT4& specular )
{
	m_trait.m_specular = specular;
}

void CSurface::SetRoughness( const float roughness )
{
	m_trait.m_roughness = roughness;
}

void CSurface::SetSpeculaPower( const float speculaPower )
{
	m_trait.m_specularPower = speculaPower;
}

void CSurface::SetTextureName( const String& textureName )
{
	m_textureName = textureName;
}