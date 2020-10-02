#pragma once

#include "PrimitiveComponent.h"

class StaticMesh;

class StaticMeshComponent : public PrimitiveComponent
{
public:
	using PrimitiveComponent::PrimitiveComponent;

	void SetStaticMesh( StaticMesh* pStaticMesh );

private:
	StaticMesh* m_pStaticMesh;
};