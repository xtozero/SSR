#pragma once

class PrimitiveComponent;
class RenderViewGroup;

enum class SHADING_METHOD
{
	Forward,
};

class IScene
{
public:
	virtual void AddPrimitive( PrimitiveComponent* primitive ) = 0;
	virtual void RemovePrimitive( PrimitiveComponent* primitive ) = 0;

	virtual void DrawScene( const RenderViewGroup& views ) = 0;

	virtual SHADING_METHOD ShadingMethod( ) const = 0;

	virtual ~IScene() = default;
};