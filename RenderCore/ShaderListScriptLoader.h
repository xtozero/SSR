#pragma once

#include "common.h"

class IRenderer;

class CShaderListScriptLoader
{
public:
	bool LoadShadersFromScript( IRenderer* const pRenderer );

	CShaderListScriptLoader( );
	~CShaderListScriptLoader( );

private:
	bool LoadShader( IRenderer* const pRenderer, const String& filePath, const String& profile );
};

