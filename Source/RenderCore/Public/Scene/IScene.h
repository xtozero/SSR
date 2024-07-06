#pragma once

#include "SparseArray.h"

namespace logic
{
	class HemisphereLightComponent;
	class LightComponent;
	class PrimitiveComponent;
	class TexturedSkyComponent;
	class VolumetricCloudComponent;
	class VolumetricFogComponent;
	class World;
}

namespace rendercore
{
	class PrimitiveSceneInfo;
	class RenderViewGroup;
	class Scene;
	class ScenePrimitiveBuffer;
	class ShaderArguments;
	class SkyAtmospherePorxy;

	enum class ShadingMethod : uint8
	{
		Forward,
	};

	class IScene
	{
	public:
		virtual logic::World* GetWorld() const = 0;

		virtual void AddPrimitive( logic::PrimitiveComponent* primitive ) = 0;
		virtual void RemovePrimitive( logic::PrimitiveComponent* primitive ) = 0;
		virtual void UpdatePrimitiveTransform( logic::PrimitiveComponent* primitive ) = 0;
		virtual SparseArray<PrimitiveSceneInfo*>& Primitives() = 0;
		virtual const SparseArray<PrimitiveSceneInfo*>& Primitives() const = 0;

		virtual void AddTexturedSkyComponent( logic::TexturedSkyComponent* texturedSky ) = 0;
		virtual void RemoveTexturedSkyComponent( logic::TexturedSkyComponent* texturedSky ) = 0;

		virtual void AddSkyAtmosphere( SkyAtmospherePorxy* skyAtmosphereProxy ) = 0;
		virtual void RemoveAtomosphere( SkyAtmospherePorxy* skyAtmosphereProxy ) = 0;

		virtual void AddVolumetricCloud( logic::VolumetricCloudComponent* volumetricCloud ) = 0;
		virtual void RemoveVolumetricCloud( logic::VolumetricCloudComponent* volumetricCloud ) = 0;

		virtual void AddVolumetricFog( logic::VolumetricFogComponent* volumetricFog ) = 0;
		virtual void RemoveVolumetricFog( logic::VolumetricFogComponent* volumetricFog ) = 0;

		virtual void AddHemisphereLightComponent( logic::HemisphereLightComponent* light ) = 0;
		virtual void RemoveHemisphereLightComponent( logic::HemisphereLightComponent* light ) = 0;

		virtual void AddLight( logic::LightComponent* light ) = 0;
		virtual void RemoveLight( logic::LightComponent* light ) = 0;

		virtual ShaderArguments& GetViewShaderArguments() = 0;
		virtual const ShaderArguments& GetViewShaderArguments() const = 0;

		virtual ScenePrimitiveBuffer& GpuPrimitiveInfo() = 0;
		virtual const ScenePrimitiveBuffer& GpuPrimitiveInfo() const = 0;

		virtual ShadingMethod GetShadingMethod() const = 0;

		virtual Scene* GetRenderScene() = 0;

		virtual void OnBeginSceneRendering() = 0;

		virtual uint64 GetNumFrame() const = 0;

		virtual ~IScene() = default;
	};
}
