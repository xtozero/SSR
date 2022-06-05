#pragma once

#include "BoxSphereBounds.h"
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

enum class Collider
{
	None = -1,
	Sphere,
	Aabb,
	Obb,
	Count,
};

class ICollider
{
public:
	virtual void Update( const Vector& scaling, const Quaternion& rotation, const Vector& translation, ICollider* original ) = 0;
	virtual float Intersect( const CRay& ray ) const = 0;
	virtual uint32 Intersect( const Frustum& frustum ) const = 0;
	virtual BoxSphereBounds Bounds() const = 0;
	virtual Collider GetType() const = 0;

	virtual ~ICollider( ) = default;
};