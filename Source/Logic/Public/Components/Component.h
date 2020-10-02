#pragma once

class CGameObject;

class Component
{
public:
	Component( CGameObject* pOwner );
	
	virtual void ThinkComponent( [[maybe_unused]]float elapsedTime ) {};

private:
	CGameObject* m_pOwner = nullptr;
};
