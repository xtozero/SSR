#pragma once

#include "common.h"
#include "GameObject.h"
#include "Math/Vector4.h"

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

	void SetDiffuseColor( const ColorF& diffuseColor );
	void SetSpecularColor( const ColorF& specularColor );
	void SetCastShadow( bool castShadow );

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

	const Vector& Direction( ) const;

	DirectionalLight( );

private:
	DirectionalLightComponent* m_directionalLightComponent = nullptr;
};

class HemisphereLight : public CGameObject
{
public:
	virtual void LoadProperty( CGameLogic& gameLogic, const JSON::Value& json ) override;

	void SetLowerColor( const ColorF& color );
	void SetUpperColor( const ColorF& color );

	HemisphereLight( );

private:
	HemisphereLightComponent* m_hemisphereLightComponent = nullptr;
};