#pragma once

#include "common.h"
#include "PrimitiveProxy.h"

class StaticMesh;
class StaticMeshComponent;

class StaticMeshPrimitiveProxy : public PrimitiveProxy
{
public:
	 RENDERCORE_DLL StaticMeshPrimitiveProxy( const StaticMeshComponent& component );

private:
	StaticMesh* m_pStaticMesh = nullptr;
};