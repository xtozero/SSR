#pragma once

#include "Core/ThinkFunction.h"
#include "Reflection.h"

class CGameObject;
class World;

namespace json
{
	class Value;
}

class Component
{
	GENERATE_CLASS_TYPE_INFO( Component )

public:
	void RegisterComponent();
	void UnregisterComponent();

	virtual void ThinkComponent( [[maybe_unused]] float elapsedTime ) {}

	void RecreateRenderState();
	virtual void SendRenderTransform();

	void UpdateState();
	void MarkRenderStateDirty();

	void MarkRenderTransformDirty();

	void RegisterThinkFunction();
	void UnRegisterThinkFunction();

	CGameObject* GetOwner() const;

	virtual void DestroyComponent();

	virtual void LoadProperty( [[maybe_unused]] const json::Value& json ) {}

	Component( CGameObject* pOwner, const char* name );
	virtual ~Component() = default;

protected:
	virtual bool ShouldCreateRenderState() const;
	virtual void CreateRenderState();
	virtual void RemoveRenderState();

	bool PhysicsStateCreated() const;
	void CreatePhysicsState();
	void DestroyPhysicsState();
	virtual bool ShouldCreatePhysicsState() const;
	virtual void OnCreatePhysicsState();
	virtual void OnDestroyPhysicsState();

	World* m_pWorld = nullptr;

	ComponentThinkFunction m_think;

private:
	void RegisterComponent( World* pWorld );

	CGameObject* m_pOwner = nullptr;

	bool m_renderStateCreated = false;
	bool m_renderStateDirty = false;
	bool m_renderTransformDirty = false;
	bool m_markForUpdateState = false;
	bool m_markForSendRenderTransform = false;
	bool m_physicsStateCreated = false;
};
