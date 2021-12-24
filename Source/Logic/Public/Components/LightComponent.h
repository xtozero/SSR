#pragma once

#include "common.h"
#include "Components/SceneComponent.h"
#include "Math/CXMFloat.h"

class HemisphereLightProxy;
class LightProxy;

enum class ShadowQuility
{
	Low = 0,
	Mid,
	High,
};

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

	bool& CastShadow( )
	{
		return m_castShadow;
	}

	bool CastShadow() const
	{
		return m_castShadow;
	}

	ShadowQuility GetShadowQuility( ) const
	{
		return m_shadowQuility;
	}

	LightProxy* m_lightProxy = nullptr;

private:
	CXMFLOAT4 m_diffuse;
	CXMFLOAT4 m_specular;

	bool m_castShadow = false;
	ShadowQuility m_shadowQuility = ShadowQuility::High;
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

class HemisphereLightComponent : public SceneComponent
{
public:
	using SceneComponent::SceneComponent;

	virtual HemisphereLightProxy* CreateProxy( ) const;

	void SetLowerColor( const CXMFLOAT4& color )
	{
		m_lowerHemisphereColor = color;
	}

	const CXMFLOAT4& LowerColor( ) const
	{
		return m_lowerHemisphereColor;
	}

	void SetUpperColor( const CXMFLOAT4& color )
	{
		m_upperHemisphereColor = color;
	}

	const CXMFLOAT4& UpperColor( ) const
	{
		return m_upperHemisphereColor;
	}

	CXMFLOAT3 UpVector( ) const;

	HemisphereLightProxy*& Proxy( )
	{
		return m_proxy;
	}

protected:
	virtual bool ShouldCreateRenderState( ) const override;
	virtual void CreateRenderState( ) override;
	virtual void RemoveRenderState( ) override;

private:
	HemisphereLightProxy* m_proxy = nullptr;
	CXMFLOAT4 m_lowerHemisphereColor;
	CXMFLOAT4 m_upperHemisphereColor;
};
