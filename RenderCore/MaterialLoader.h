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
	bool CreateScriptedMaterial( IRenderer& renderer, std::shared_ptr<KeyValue> pMaterial );
};

std::unique_ptr<CMaterialLoader> CreateMaterialLoader( );

