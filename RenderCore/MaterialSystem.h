#pragma once

#include "common.h"
#include <map>

class IMaterial;

class MaterialSystem
{
private:
	std::map<String, IMaterial*> m_materials;

public:
	static MaterialSystem* GetInstance( );

	void RegisterMaterial( const TCHAR* pName, IMaterial* pMaterial );
	IMaterial* SearchMaterialByName( const TCHAR* pName );

	MaterialSystem( );
	virtual ~MaterialSystem( );
};

#define REGISTER_MATERIAL( T, X ) \
	static T X; \
	X.Init( ); \
	MaterialSystem::GetInstance( )->RegisterMaterial( _T( #X ), &X )