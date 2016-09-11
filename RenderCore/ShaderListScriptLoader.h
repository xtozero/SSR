#pragma once

#include "common.h"

class IRenderer;

class CShaderListScriptLoader
{
public:
	bool LoadShadersFromScript( IRenderer& renderer );

	CShaderListScriptLoader( );
	~CShaderListScriptLoader( );

private:
	bool LoadShader( IRenderer& renderer, const String& filePath, const String& profile );
};

