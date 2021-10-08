#pragma once

#include "common.h"
#include "GameObject.h"

#include <array>

class DirectionalLightComponent;
class HemisphereLightComponent;
class LightComponent;

enum class LIGHT_TYPE
{
	NONE = -1,
	DIRECTINAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT
};

class Light : public CGameObject
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

	virtual ~Light( ) = default;

private:
	LightComponent& GetLightComponent( );

	LightComponent* m_component = nullptr;
};

class DirectionalLight : public Light
{
public:
	virtual const LIGHT_TYPE GetType( ) const override;

	virtual void LoadProperty( CGameLogic& gameLogic, const JSON::Value& json ) override;

	const CXMFLOAT3& Direction( ) const;

	DirectionalLight( );

private:
	DirectionalLightComponent* m_directionalLightComponent = nullptr;
};

class HemisphereLight : public CGameObject
{
public:
	virtual void LoadProperty( CGameLogic& gameLogic, const JSON::Value& json ) override;

	void SetLowerColor( const CXMFLOAT4& color );
	void SetUpperColor( const CXMFLOAT4& color );

	HemisphereLight( );

private:
	HemisphereLightComponent* m_hemisphereLightComponent = nullptr;
};