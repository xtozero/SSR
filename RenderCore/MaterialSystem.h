#pragma once

#include "common.h"
#include "MaterialCommon.h"

#include "../shared/Util.h"

#include <map>

class IBuffer;
class IMaterial;

class MaterialSystem
{
private:
	std::map<String, std::unique_ptr<IMaterial>> m_materials;
	MatConstantBuffers m_constantBuffers;

public:
	static MaterialSystem* GetInstance( );

	void RegisterMaterial( const TCHAR* pName, Owner<IMaterial*> pMaterial );
	void RegisterConstantBuffer( UINT type, IBuffer* pConstantBuffer );
	IMaterial* SearchMaterialByName( const TCHAR* pName );

	virtual ~MaterialSystem( );
};

#define REGISTER_MATERIAL( pRenderer, T, X ) \
	T* X = new T; \
	X->Init( pRenderer ); \
	MaterialSystem::GetInstance( )->RegisterMaterial( _T( #X ), X )