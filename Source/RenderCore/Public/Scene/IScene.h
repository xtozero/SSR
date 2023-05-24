#pragma once

#include "SparseArray.h"

class HemisphereLightComponent;
class LightComponent;
class PrimitiveComponent;
class TexturedSkyComponent;
class VolumetricCloudComponent;
class VolumetricFogComponent;

namespace rendercore
{
	class PrimitiveSceneInfo;
	class RenderViewGroup;
	class Scene;
	class ScenePrimitiveBuffer;
	class SceneViewConstantBuffer;
	class SkyAtmospherePorxy;

	enum class ShadingMethod : uint8
	{
		Forward,
	};

	class IScene
	{
	public:
		virtual void AddPrimitive( PrimitiveComponent* primitive ) = 0;
		virtual void RemovePrimitive( PrimitiveComponent* primitive ) = 0;
		virtual void UpdatePrimitiveTransform( PrimitiveComponent* primitive ) = 0;
		virtual SparseArray<PrimitiveSceneInfo*>& Primitives() = 0;
		virtual const SparseArray<PrimitiveSceneInfo*>& Primitives() const = 0;

		virtual void AddTexturedSkyComponent( TexturedSkyComponent* texturedSky ) = 0;
		virtual void RemoveTexturedSkyComponent( TexturedSkyComponent* texturedSky ) = 0;

		virtual void AddSkyAtmosphere( SkyAtmospherePorxy* skyAtmosphereProxy ) = 0;
		virtual void RemoveAtomosphere( SkyAtmospherePorxy* skyAtmosphereProxy ) = 0;

		virtual void AddVolumetricCloud( VolumetricCloudComponent* volumetricCloud ) = 0;
		virtual void RemoveVolumetricCloud( VolumetricCloudComponent* volumetricCloud ) = 0;

		virtual void AddVolumetricFog( VolumetricFogComponent* volumetricFog ) = 0;
		virtual void RemoveVolumetricFog( VolumetricFogComponent* volumetricFog ) = 0;

		virtual void AddHemisphereLightComponent( HemisphereLightComponent* light ) = 0;
		virtual void RemoveHemisphereLightComponent( HemisphereLightComponent* light ) = 0;

		virtual void AddLight( LightComponent* light ) = 0;
		virtual void RemoveLight( LightComponent* light ) = 0;

		virtual SceneViewConstantBuffer& SceneViewConstant() = 0;
		virtual const SceneViewConstantBuffer& SceneViewConstant() const = 0;

		virtual ScenePrimitiveBuffer& GpuPrimitiveInfo() = 0;
		virtual const ScenePrimitiveBuffer& GpuPrimitiveInfo() const = 0;

		virtual ShadingMethod GetShadingMethod() const = 0;

		virtual Scene* GetRenderScene() = 0;

		virtual void OnBeginSceneRendering() = 0;

		virtual uint64 GetNumFrame() const = 0;

		virtual ~IScene() = default;
	};
}
