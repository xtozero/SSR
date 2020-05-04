#pragma once

#include "Material.h"

#include <map>
#include <memory>
#include <vector>

class IRenderer;
class KeyValue;

class CMaterialManager
{
public:
	void OnDeviceLost( );
	bool LoadMaterials( IRenderer& renderer );
	Material SearchMaterialByName( const TCHAR* pName );
	const CMaterial& GetConcrete( Material handle );

private:
	void CreateScriptedMaterial( IRenderer& renderer, const KeyValue& keyValue );

	std::vector<CMaterial> m_materials;
	std::map<String, Material> m_matLUT;
	BYTE* m_freeMaterial = nullptr;
};
