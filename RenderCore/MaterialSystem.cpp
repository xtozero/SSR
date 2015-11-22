#include "stdafx.h"
#include "Material.h"
#include "MaterialSystem.h"

MaterialSystem* MaterialSystem::GetInstance( )
{
	static MaterialSystem materialSystem;
	return &materialSystem;
}

void MaterialSystem::RegisterMaterial( const TCHAR* pName, IMaterial* pMaterial )
{
	if ( pName && pMaterial )
	{
		std::map<String, IMaterial*>::iterator finded = m_materials.find( pName );

		if ( finded == m_materials.end( ) )
		{
			m_materials.emplace( pName, pMaterial );
		}
	}
}

IMaterial* MaterialSystem::SearchMaterialByName( const TCHAR* pName )
{
	if ( !pName )
	{
		return nullptr;
	}

	std::map<String, IMaterial*>::iterator finded = m_materials.find( pName );

	if ( finded != m_materials.end( ) )
	{
		return finded->second;
	}

	return nullptr;
}

MaterialSystem::MaterialSystem( )
{
}


MaterialSystem::~MaterialSystem( )
{
	//���׸��� ����� ������ DECLARE_MATERIAL�� �Ұ� �ȱ׷��� �����Ͱ� �������� ����.
	m_materials.clear( );
}