#pragma once

#include "common.h"

class IRenderer;
class IMeshBuilder;

RENDERCORE_FUNC_DLL IRenderer* CreateDirect3D11Renderer( );
RENDERCORE_FUNC_DLL IMeshBuilder* GetMeshBuilder( );