#pragma once

#include <map>
#include <memory>

class IMaterial;
class IRenderer;
class KeyValue;

class CMaterialManager
{
public:
	void OnDeviceLost( );
	bool LoadMaterials( IRenderer& renderer );
	void RegisterMaterial( const TCHAR* pName, std::unique_ptr<IMaterial> pMaterial );
	IMaterial* SearchMaterialByName( const TCHAR* pName );

private:
	bool CreateScriptedMaterial( IRenderer& renderer, const KeyValue& keyValue );

	std::map<String, std::unique_ptr<IMaterial>> m_materials;
};
