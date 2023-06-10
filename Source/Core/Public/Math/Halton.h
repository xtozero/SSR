#pragma once

#include "SizedTypes.h"
#include "Vector.h"
#include "Vector2.h"

class Halton
{
public:
	float Sample( uint64 index, uint32 dim ) const;
	Vector2 Sample2D( uint64 index ) const;
	Vector Sample3D( uint64 index ) const;

private:
	float RadicalInverse( uint32 baseIndex, uint64 a ) const;
};