#pragma once

#include "SizedTypes.h"

#include <memory>
#include <vector>

class Frustum;
class CRay;
struct MeshData;
struct Quaternion;
struct Vector;

namespace COLLISION
{
	enum TYPE
	{
		OUTSIDE = 0,
		INSIDE,
		INTERSECTION,
	};
}

class ICollider
{
public:
	virtual void CalcMeshBounds( const MeshData& mesh ) = 0;
	virtual void Update( const Vector& scaling, const Quaternion& rotation, const Vector& translation, ICollider* original ) = 0;
	virtual void CalcSubMeshBounds( std::vector<std::unique_ptr<ICollider>>& subColliders ) = 0;
	virtual float Intersect( const CRay& ray ) const = 0;
	virtual uint32 Intersect( const Frustum& frustum ) const = 0;

	virtual ~ICollider( ) = default;
};