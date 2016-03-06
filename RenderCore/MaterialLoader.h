#pragma once

#include <memory>

class IMaterial;
class KeyValue;

class CMaterialLoader
{
public:
	bool LoadMaterials( );

private:
	bool CreateScriptedMaterial( std::shared_ptr<KeyValue> pMaterial );
};

std::unique_ptr<CMaterialLoader> CreateMaterialLoader( );

