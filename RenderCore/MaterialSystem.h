#pragma once

#include "common.h"
#include "MaterialCommon.h"
#include <map>

class IBuffer;
class IMaterial;

class MaterialSystem
{
private:
	std::map<String, std::shared_ptr<IMaterial>> m_materials;
	MatConstantBuffers m_constantBuffers;

public:
	static MaterialSystem* GetInstance( );

	void RegisterMaterial( const TCHAR* pName, std::shared_ptr<IMaterial> pMaterial );
	void RegisterConstantBuffer( UINT type, const std::shared_ptr<IBuffer>& pConstantBuffer );
	std::shared_ptr<IMaterial> SearchMaterialByName( const TCHAR* pName );

	MaterialSystem( );
	virtual ~MaterialSystem( );
};

#define REGISTER_MATERIAL( T, X ) \
	std::shared_ptr<T> X = std::make_shared<T>(); \
	X->Init( ); \
	MaterialSystem::GetInstance( )->RegisterMaterial( _T( #X ), X )