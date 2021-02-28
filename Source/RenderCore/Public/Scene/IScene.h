#pragma once

#include <vector>

class PrimitiveComponent;
class PrimitiveSceneInfo;
class RenderViewGroup;
class Scene;
class SceneViewConstantBuffer;

enum class SHADING_METHOD
{
	Forward,
};

class IScene
{
public:
	virtual void AddPrimitive( PrimitiveComponent* primitive ) = 0;
	virtual void RemovePrimitive( PrimitiveComponent* primitive ) = 0;
	virtual std::vector<PrimitiveSceneInfo*>& Primitives( ) = 0;
	virtual const std::vector<PrimitiveSceneInfo*>& Primitives( ) const = 0;

	virtual SceneViewConstantBuffer& SceneViewConstant( ) = 0;
	virtual void DrawScene( const RenderViewGroup& views ) = 0;

	virtual SHADING_METHOD ShadingMethod( ) const = 0;

	virtual Scene* GetRenderScene( ) = 0;

	virtual ~IScene() = default;
};