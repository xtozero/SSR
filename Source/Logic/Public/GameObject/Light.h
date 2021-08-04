#pragma once

#include "common.h"
#include "GameObject.h"

#include <array>

class DirectionalLightComponent;
class LightComponent;

enum class LIGHT_TYPE
{
	NONE = -1,
	AMBIENT_LIGHT,
	DIRECTINAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT
};

struct LightTrait
{
	LIGHT_TYPE				m_type = LIGHT_TYPE::NONE;
	bool					m_isOn = false;
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

class CLight : public CGameObject
{
public:
	virtual const LIGHT_TYPE GetType( ) const = 0;

	virtual void LoadProperty( CGameLogic& gameLogic, const JSON::Value& json ) override;
	//const bool IsOn( ) const;
	//CXMFLOAT3 GetDirection( ) const;

	//void SetOnOff( const bool on );
	//void SetRange( const float range );
	//void SetFallOff( const float fallOff );
	//void SetConeProperty( const float theta, const float phi );
	//void SetDiection( const CXMFLOAT3& direction );
	//void SetAttenuation( const CXMFLOAT3& attenuation );
	void SetDiffuseColor( const CXMFLOAT4& diffuseColor );
	void SetSpecularColor( const CXMFLOAT4& specularColor );

	virtual ~CLight( ) = default;

private:
	LightComponent& GetLightComponent( );

	LightComponent* m_component = nullptr;
};

class DirectionalLight : public CLight
{
public:
	virtual const LIGHT_TYPE GetType( ) const override;

	virtual void LoadProperty( CGameLogic& gameLogic, const JSON::Value& json ) override;

	const CXMFLOAT3& Direction( ) const;

	DirectionalLight( );

private:
	DirectionalLightComponent* m_directionalLightComponent = nullptr;
};