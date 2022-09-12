#pragma once

#include "common.h"
#include "Math/Vector.h"
#include "Math/Vector4.h"

class BoxSphereBounds;
class DirectionalLightComponent;
class HemisphereLightComponent;
class LightComponent;

namespace rendercore
{
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
		virtual LIGHT_TYPE LightType() const = 0;
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

		RENDERCORE_DLL LightProxy( const LightComponent& component );
		virtual ~LightProxy() = default;

	protected:
		friend Scene;
		LightSceneInfo* m_lightSceneInfo = nullptr;

		ColorF m_diffuse;
		ColorF m_specular;

		bool m_castShadow = false;
		bool m_usedAsAtmosphereSunLight = false;
	};

	class DirectionalLightProxy : public LightProxy
	{
	public:
		virtual LIGHT_TYPE LightType() const override
		{
			return LIGHT_TYPE::DIRECTINAL_LIGHT;
		}

		virtual LightProperty GetLightProperty() const override;

		virtual bool AffactsBounds( const BoxSphereBounds& bounds ) const override;

		RENDERCORE_DLL DirectionalLightProxy( const DirectionalLightComponent& component );

	private:
		Vector m_direction;
	};

	class HemisphereLightProxy
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
