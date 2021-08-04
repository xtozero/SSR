#pragma once

#include "common.h"
#include "Components/SceneComponent.h"
#include "Math/CXMFloat.h"

class LightProxy;

class LightComponent : public SceneComponent
{
public:
	using SceneComponent::SceneComponent;

	virtual LightProxy* CreateProxy( ) const = 0;

	void SetDiffuseColor( const CXMFLOAT4& diffuseColor );
	const CXMFLOAT4& DiffuseColor( ) const
	{
		return m_diffuse;
	}

	void SetSpecularColor( const CXMFLOAT4& specularColor );
	const CXMFLOAT4& SpecularColor( ) const
	{
		return m_specular;
	}

	LightProxy* m_lightProxy = nullptr;

private:
	CXMFLOAT4 m_diffuse;
	CXMFLOAT4 m_specular;
};

class DirectionalLightComponent : public LightComponent
{
public:
	using LightComponent::LightComponent;

	virtual LightProxy* CreateProxy( ) const override;

	void SetDirection( const CXMFLOAT3& direction );
	const CXMFLOAT3& Direction( ) const
	{
		return m_direction;
	}

protected:
	virtual bool ShouldCreateRenderState( ) const override;
	virtual void CreateRenderState( ) override;
	virtual void RemoveRenderState( ) override;

private:
	CXMFLOAT3 m_direction;
};