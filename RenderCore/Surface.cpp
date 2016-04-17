#include "stdafx.h"
#include "Surface.h"

void CSurface::SetAmbient( const D3DXCOLOR& ambient )
{
	m_trait.m_ambient = ambient;
}

void CSurface::SetDiffuse( const D3DXCOLOR& diffuse )
{
	m_trait.m_diffuse = diffuse;
}

void CSurface::SetSpecular( const D3DXCOLOR& specular )
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