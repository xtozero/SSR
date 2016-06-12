#pragma once

#include <d3dX9math.h>

#include "common.h"

struct ID3D11DeviceContext;

namespace Microsoft
{
	namespace WRL
	{
		template <typename T> class ComPtr;
	}
}

class SurfaceTrait
{
public:
	SurfaceTrait( ) :
		m_ambient( 1.f, 1.f, 1.f, 1.0f ),
		m_diffuse( 1.f, 1.f, 1.f, 1.0f ),
		m_specular( 1.f, 1.f, 1.f, 1.0f ),
		m_roughness( 0.01f ),
		m_specularPower( 0.f ),
		m_pedding{ 0.f, 0.f }
	{

	}
	~SurfaceTrait( ) = default;

	D3DXCOLOR	m_ambient;
	D3DXCOLOR	m_diffuse;
	D3DXCOLOR	m_specular;
	float		m_roughness;
	float		m_specularPower;
	float		m_pedding[2];
};

class ISurface
{
public:
	virtual void SetAmbient( const D3DXCOLOR& ambient ) = 0;
	virtual void SetDiffuse( const D3DXCOLOR& diffuse ) = 0;
	virtual void SetSpecular( const D3DXCOLOR& specular ) = 0;
	virtual void SetRoughness( const float roughness ) = 0;
	virtual void SetSpeculaPower( const float speculaPower ) = 0;
	virtual void SetTextureName( const String& textureName ) = 0;
	virtual const String& GetTextureName( ) const = 0;
	virtual const SurfaceTrait* GetTrait( ) const = 0;

	ISurface( ) = default;
	virtual ~ISurface( ) = default;
};