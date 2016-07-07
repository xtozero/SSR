#pragma once

#include <memory>

class IMaterial;
class IRenderer;
class KeyValue;

class CMaterialLoader
{
public:
	bool LoadMaterials( IRenderer* pRenderer );

private:
	bool CreateScriptedMaterial( IRenderer* pRenderer, std::shared_ptr<KeyValue> pMaterial );
};

std::unique_ptr<CMaterialLoader> CreateMaterialLoader( );

