#pragma once

#include "common.h"

class IRenderer;
class IModelBuilder;

RENDERCORE_FUNC_DLL IRenderer* CreateDirect3D11Renderer( );