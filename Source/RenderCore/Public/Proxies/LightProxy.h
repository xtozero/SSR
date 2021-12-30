#pragma once

#include "common.h"
#include "Math/CXMFloat.h"

class BoxSphereBounds;
class DirectionalLightComponent;
class HemisphereLightComponent;
class LightComponent;
class LightSceneInfo;
class Scene;

struct RenderView;

enum class LIGHT_TYPE
{
	NONE = -1,
	DIRECTINAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT
};

struct LightProperty
{
	LIGHT_TYPE				m_type = LIGHT_TYPE::NONE;
	float					m_theta = 0.f;
	float					m_phi = 0.f;
	CXMFLOAT3				m_direction;
	float					m_range = 0.f;
	float					m_fallOff = 0.f;
	CXMFLOAT3				m_attenuation;
	CXMFLOAT4				m_position;
	CXMFLOAT4				m_diffuse;
	CXMFLOAT4				m_specular;
};

class LightProxy
{
public:
	virtual LIGHT_TYPE LightType( ) const = 0;
	virtual LightProperty GetLightProperty( ) const = 0;
	virtual bool AffactsBounds( const BoxSphereBounds& bounds ) const = 0;

	bool& CastShadow( )
	{
		return m_castShadow;
	}

	bool CastShadow( ) const
	{
		return m_castShadow;
	}

	RENDERCORE_DLL LightProxy( const LightComponent& component );
	virtual ~LightProxy() = default;

protected:
	friend Scene;
	LightSceneInfo* m_lightSceneInfo = nullptr;

	CXMFLOAT4 m_diffuse;
	CXMFLOAT4 m_specular;

	bool m_castShadow = false;
};

class DirectionalLightProxy : public LightProxy
{
public:
	virtual LIGHT_TYPE LightType( ) const override
	{
		return LIGHT_TYPE::DIRECTINAL_LIGHT;
	}

	virtual LightProperty GetLightProperty( ) const override;

	virtual bool AffactsBounds( const BoxSphereBounds& bounds ) const override;

	RENDERCORE_DLL DirectionalLightProxy( const DirectionalLightComponent& component );

private:
	CXMFLOAT3 m_direction;
};

class HemisphereLightProxy
{
public:
	RENDERCORE_DLL HemisphereLightProxy( const CXMFLOAT4& lowerColor, const CXMFLOAT4& upperColor, const CXMFLOAT3& upVector );

	const CXMFLOAT4& LowerColor( ) const
	{
		return m_lowerHemisphereColor;
	}

	const CXMFLOAT4& UpperColor( ) const
	{
		return m_upperHemisphereColor;
	}

	const CXMFLOAT3& UpVector( ) const
	{
		return m_upVector;
	}

private:
	CXMFLOAT4 m_lowerHemisphereColor;
	CXMFLOAT4 m_upperHemisphereColor;
	CXMFLOAT3 m_upVector;
};