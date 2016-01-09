#pragma once
#include "Material.h"

class WireFrame : public Material<WireFrame>
{
public:
	virtual void Init( ) override;

	WireFrame( );
	virtual ~WireFrame( );
};