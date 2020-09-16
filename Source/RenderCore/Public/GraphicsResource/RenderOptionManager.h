#pragma once

#include "Util.h"

#include <string>

struct RenderOption;

class IRenderOptionManager
{
public:
	virtual bool Bootup( ) = 0;
	virtual const RenderOption* GetRenderOption( const std::string& name ) const = 0;

	virtual ~IRenderOptionManager( ) = default;
};

Owner<IRenderOptionManager*> CreateRenderOptionManager( );
void DestoryRenderOptionManager( Owner<IRenderOptionManager*> pRenderOptionManager );