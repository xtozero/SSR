#pragma once
#include "ISurface.h"

class CSurface : public ISurface
{
public:
	virtual void SetAmbient( const D3DXCOLOR& ambient ) override;
	virtual void SetDiffuse( const D3DXCOLOR& diffuse ) override;
	virtual void SetSpecular( const D3DXCOLOR& specular ) override;
	virtual void SetRoughness( const float roughness ) override;
	virtual void SetSpeculaPower( const float speculaPower ) override;
	virtual void SetTextureName( const String& textureName ) override;
	virtual const String& GetTextureName( ) const override { return m_textureName; };
	virtual const SurfaceTrait* GetTrait( ) const override { return &m_trait; };

	CSurface( ) = default;
	virtual ~CSurface( ) = default;

private:
	SurfaceTrait m_trait;
	String m_textureName;
};