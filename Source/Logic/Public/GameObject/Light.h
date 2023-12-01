#pragma once

#include "common.h"
#include "GameObject.h"
#include "Math/Vector4.h"

#include <array>

namespace logic
{
	class DirectionalLightComponent;
	class HemisphereLightComponent;
	class LightComponent;
	class PointLightComponent;
	class SpotLightComponent;

	enum class LightType : uint8
	{
		Directional = 0,
		Point,
		Spot
	};

	class Light : public CGameObject
	{
		GENERATE_CLASS_TYPE_INFO( Light )

	public:
		virtual const LightType GetType() const = 0;

		void SetDiffuseColor( const ColorF& diffuseColor );
		void SetSpecularColor( const ColorF& specularColor );
		void SetCastShadow( bool castShadow );

	private:
		LightComponent& GetLightComponent();

		LightComponent* m_component = nullptr;
	};

	class DirectionalLight : public Light
	{
	public:
		virtual const LightType GetType() const override;

		const Vector& Direction() const;

		DirectionalLight();

	private:
		DirectionalLightComponent* m_directionalLightComponent = nullptr;
	};

	class PointLight : public Light
	{
	public:
		virtual const LightType GetType() const override;

		PointLight();

	private:
		PointLightComponent* m_pointLightComponent = nullptr;
	};

	class SpotLight : public Light
	{
	public:
		virtual const LightType GetType() const override;

		const Vector& Direction() const;

		SpotLight();

	private:
		SpotLightComponent* m_spotLightComponent = nullptr;
	};

	class HemisphereLight : public CGameObject
	{
		GENERATE_CLASS_TYPE_INFO( HemisphereLight )

	public:
		void SetLowerColor( const ColorF& color );
		void SetUpperColor( const ColorF& color );

		HemisphereLight();

	private:
		HemisphereLightComponent* m_hemisphereLightComponent = nullptr;
	};
}
