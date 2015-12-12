#include "stdafx.h"
#include "Material.h"
#include "MaterialSystem.h"

MaterialSystem* MaterialSystem::GetInstance( )
{
	static MaterialSystem materialSystem;
	return &materialSystem;
}

void MaterialSystem::RegisterMaterial( const TCHAR* pName, std::shared_ptr<IMaterial> pMaterial )
{
	if ( pName && pMaterial )
	{
		auto finded = m_materials.find( pName );

		if ( finded == m_materials.end( ) )
		{
			m_materials.emplace( pName, pMaterial );
		}
	}
}

std::shared_ptr<IMaterial> MaterialSystem::SearchMaterialByName( const TCHAR* pName )
{
	if ( !pName )
	{
		return nullptr;
	}

	auto finded = m_materials.find( pName );

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
	//���׸��� ����� ������ REGISTER_MATERIAL�� �Ұ� �ȱ׷��� �����Ͱ� �������� ����.
	m_materials.clear( );
}