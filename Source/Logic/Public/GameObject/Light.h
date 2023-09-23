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

	enum class LightType : uint8
	{
		None = 0,
		Directional,
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
