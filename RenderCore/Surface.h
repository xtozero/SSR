#pragma once

#include "CommonRenderer/ISurface.h"

class Surface : public ISurface
{
public:
	virtual void SetAmbient( const CXMFLOAT4& ambient ) override;
	virtual void SetDiffuse( const CXMFLOAT4& diffuse ) override;
	virtual void SetSpecular( const CXMFLOAT4& specular ) override;
	virtual void SetRoughness( const float roughness ) override;
	virtual void SetSpeculaPower( const float speculaPower ) override;
	virtual void SetTextureName( const String& textureName ) override;
	virtual const String& GetTextureName( ) const override { return m_textureName; };
	virtual const SurfaceTrait& GetTrait( ) const override { return m_trait; };

	Surface( ) = default;
	virtual ~Surface( ) = default;

private:
	SurfaceTrait m_trait;
	String m_textureName;
};