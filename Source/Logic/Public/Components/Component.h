#pragma once

#include "Core/ThinkFunction.h"
#include "Reflection.h"

class CGameObject;
class World;

class Component
{
	GENERATE_CLASS_TYPE_INFO( Component )

public:
	void RegisterComponent();
	void UnregisterComponent();

	virtual void ThinkComponent( [[maybe_unused]] float elapsedTime ) {};

	void RecreateRenderState();

	void UpdateState();
	void MarkRenderStateDirty();

	void RegisterThinkFunction();
	void UnRegisterThinkFunction();

	explicit Component( CGameObject* pOwner );
	virtual ~Component() = default;

protected:
	virtual bool ShouldCreateRenderState() const;
	virtual void CreateRenderState();
	virtual void RemoveRenderState();

	World* m_pWorld = nullptr;

private:
	void RegisterComponent( World* pWorld );

	CGameObject* m_pOwner = nullptr;

	bool m_renderStateCreated = false;
	bool m_renderStateDirty = false;
	bool m_markForUpdateState = false;

	ComponentThinkFunction m_think;
};
