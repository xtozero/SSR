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
	bool CreateScriptedMaterial( IRenderer& renderer, const KeyValue* pKeyValue );
};

std::unique_ptr<CMaterialLoader> CreateMaterialLoader( );

