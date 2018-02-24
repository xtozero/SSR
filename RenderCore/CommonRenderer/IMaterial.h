#pragma once

#include <array>
#include <memory>

class IRenderer;

class IMaterial
{
public:
	virtual void Init( IRenderer& renderer ) = 0;
	virtual void Bind( IRenderer& renderer ) = 0;

	virtual ~IMaterial( ) = default;

protected:
	IMaterial( ) = default;
};