#pragma once

#include "PrimitiveComponent.h"

class StaticMesh;

class StaticMeshComponent : public PrimitiveComponent
{
public:
	using PrimitiveComponent::PrimitiveComponent;

	virtual PrimitiveProxy* CreateProxy( ) const override;

	void SetStaticMesh( StaticMesh* pStaticMesh );
	StaticMesh* GetStaticMesh( ) const { return m_pStaticMesh; }

private:
	StaticMesh* m_pStaticMesh = nullptr;
};