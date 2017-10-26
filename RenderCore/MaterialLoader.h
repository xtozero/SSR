#pragma once

#include <memory>

class IMaterial;
class IRenderer;
class KeyValue;

class CMaterialLoader
{
public:
	bool LoadMaterials( IRenderer& renderer );

private:
	bool CreateScriptedMaterial( IRenderer& renderer, const KeyValue& keyValue );
};
