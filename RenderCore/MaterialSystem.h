#pragma once

#include "common.h"
#include "CommonRenderer/IMaterial.h"

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

	void RegisterMaterial( const TCHAR* pName, std::unique_ptr<IMaterial> pMaterial );
	void RegisterConstantBuffer( UINT type, IBuffer* pConstantBuffer );
	IMaterial* SearchMaterialByName( const TCHAR* pName );

	virtual ~MaterialSystem( );
};

#define REGISTER_MATERIAL( pRenderer, T, X ) \
	std::unique_ptr<T> X = std::make_unique<T>(); \
	X->Init( pRenderer ); \
	MaterialSystem::GetInstance( )->RegisterMaterial( _T( #X ), std::move( X ) )