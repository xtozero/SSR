#pragma once

#include "common.h"
#include "../shared/Math/CXMFloat.h"

struct ID3D11DeviceContext;

namespace Microsoft
{
	namespace WRL
	{
		template <typename T> class ComPtr;
	}
}

struct SurfaceTrait
{
	CXMFLOAT4	m_ambient = { 1.f, 1.f, 1.f, 1.0f };
	CXMFLOAT4	m_diffuse = { 1.f, 1.f, 1.f, 1.0f };
	CXMFLOAT4	m_specular = { 1.f, 1.f, 1.f, 1.0f };
	float		m_roughness = 0.01f;
	float		m_specularPower = 0.f;
	float		m_pedding[2] = { 0.f, 0.f };
};

class ISurface
{
public:
	virtual void SetAmbient( const CXMFLOAT4& ambient ) = 0;
	virtual void SetDiffuse( const CXMFLOAT4& diffuse ) = 0;
	virtual void SetSpecular( const CXMFLOAT4& specular ) = 0;
	virtual void SetRoughness( const float roughness ) = 0;
	virtual void SetSpeculaPower( const float speculaPower ) = 0;
	virtual void SetTextureName( const String& textureName ) = 0;
	virtual const String& GetTextureName( ) const = 0;
	virtual const SurfaceTrait& GetTrait( ) const = 0;

	ISurface( ) = default;
	virtual ~ISurface( ) = default;
};