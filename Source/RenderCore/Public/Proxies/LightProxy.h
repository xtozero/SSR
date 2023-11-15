#pragma once

#include "ColorTypes.h"
#include "common.h"
#include "Math/Vector.h"
#include "Math/Vector4.h"
#include "SizedTypes.h"

class BoxSphereBounds;

namespace logic
{
	class DirectionalLightComponent;
	class HemisphereLightComponent;
	class LightComponent;
}

namespace rendercore
{
	class LightSceneInfo;
	class Scene;

	struct RenderView;

	enum class LightType : uint8
	{
		None = 0,
		Directional,
		Point,
		Spot
	};

	struct LightProperty
	{
		LightType				m_type = LightType::None;
		float					m_theta = 0.f;
		float					m_phi = 0.f;
		Vector					m_direction;
		float					m_range = 0.f;
		float					m_fallOff = 0.f;
		Vector					m_attenuation;
		Vector4					m_position;
		ColorF					m_diffuse;
		ColorF					m_specular;
	};

	class LightProxy
	{
	public:
		virtual LightType GetLightType() const = 0;
		virtual LightProperty GetLightProperty() const = 0;
		virtual bool AffactsBounds( const BoxSphereBounds& bounds ) const = 0;

		bool& CastShadow()
		{
			return m_castShadow;
		}

		bool CastShadow() const
		{
			return m_castShadow;
		}

		bool& IsUsedAsAtmosphereSunLight()
		{
			return m_usedAsAtmosphereSunLight;
		}

		bool IsUsedAsAtmosphereSunLight() const
		{
			return m_usedAsAtmosphereSunLight;
		}

		RENDERCORE_DLL LightProxy( const logic::LightComponent& component );
		virtual ~LightProxy() = default;

	protected:
		friend Scene;
		LightSceneInfo* m_lightSceneInfo = nullptr;

		ColorF m_diffuse;
		ColorF m_specular;

		bool m_castShadow = false;
		bool m_usedAsAtmosphereSunLight = false;
	};

	class DirectionalLightProxy final : public LightProxy
	{
	public:
		virtual LightType GetLightType() const override
		{
			return LightType::Directional;
		}

		virtual LightProperty GetLightProperty() const override;

		virtual bool AffactsBounds( const BoxSphereBounds& bounds ) const override;

		RENDERCORE_DLL DirectionalLightProxy( const logic::DirectionalLightComponent& component );

	private:
		Vector m_direction;
	};

	class HemisphereLightProxy final
	{
	public:
		RENDERCORE_DLL HemisphereLightProxy( const ColorF& lowerColor, const ColorF& upperColor, const Vector& upVector );

		const ColorF& LowerColor() const
		{
			return m_lowerHemisphereColor;
		}

		const ColorF& UpperColor() const
		{
			return m_upperHemisphereColor;
		}

		const Vector& UpVector() const
		{
			return m_upVector;
		}

	private:
		ColorF m_lowerHemisphereColor;
		ColorF m_upperHemisphereColor;
		Vector m_upVector;
	};
}
