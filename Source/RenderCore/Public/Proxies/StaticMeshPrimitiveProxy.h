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
	std::shared_ptr<const StaticMesh> m_pStaticMesh = nullptr;
};