#pragma once

#include "Math/Matrix.h"
#include "Physics/Body.h"
#include "SceneComponent.h"

class PrimitiveProxy;

class PrimitiveComponent : public SceneComponent
{
	GENERATE_CLASS_TYPE_INFO( PrimitiveComponent )

public:
	using SceneComponent::SceneComponent;

	void SetMass( float mass );

	void SetLinearDamping( float linearDamping );
	void SetAngularDamping( float angularDamping );

	virtual const Matrix& GetRenderMatrix( );
	virtual PrimitiveProxy* CreateProxy( ) const = 0;

	PrimitiveProxy* m_sceneProxy = nullptr;

protected:
	virtual bool ShouldCreateRenderState( ) const override;
	virtual void CreateRenderState( ) override;
	virtual void RemoveRenderState( ) override;

private:
	RigidBody m_rigidBody;
};