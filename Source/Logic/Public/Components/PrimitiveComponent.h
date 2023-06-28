#pragma once

#include "Math/Matrix.h"
#include "Physics/BodyInstance.h"
#include "SceneComponent.h"

namespace rendercore
{
	class PrimitiveProxy;
}

class PrimitiveComponent : public SceneComponent
{
	GENERATE_CLASS_TYPE_INFO( PrimitiveComponent )

public:
	using SceneComponent::SceneComponent;
	virtual void SendRenderTransform() override;

	virtual void LoadProperty( const json::Value& json ) override;

	void SetMass( float mass );

	void SetLinearDamping( float linearDamping );
	void SetAngularDamping( float angularDamping );

	virtual const Matrix& GetRenderMatrix();
	virtual rendercore::PrimitiveProxy* CreateProxy() const = 0;
	virtual BodySetup* GetBodySetup() = 0;

	rendercore::PrimitiveProxy* m_sceneProxy = nullptr;

protected:
	virtual bool ShouldCreateRenderState() const override;
	virtual void CreateRenderState() override;
	virtual void RemoveRenderState() override;

	virtual bool ShouldCreatePhysicsState() const;
	virtual void OnCreatePhysicsState() override;
	virtual void OnDestroyPhysicsState() override;

	BodyInstance m_bodyInstance;
};