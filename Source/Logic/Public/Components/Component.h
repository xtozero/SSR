#pragma once

class CGameObject;
class World;

class Component
{
public:
	void RegisterComponent( );
	void UnregisterComponent( );

	virtual void CreateRenderState( );
	virtual void RemoveRenderState( );
	virtual void ThinkComponent( [[maybe_unused]]float elapsedTime ) {};

	void RecreateRenderState( );

	void UpdateState( );
	void MarkRenderStateDirty( );

	Component( CGameObject* pOwner );
	virtual ~Component( ) = default;

protected:
	virtual bool ShouldCreateRenderState( ) const;

	World* m_pWorld = nullptr;

private:
	void RegisterComponent( World* pWorld );

	CGameObject* m_pOwner = nullptr;

	bool m_renderStateCreated = false;
	bool m_renderStateDirty = false;
	bool m_markForUpdateState = false;
};
