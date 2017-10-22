#include "stdafx.h"
#include "Material.h"
#include "MaterialSystem.h"

MaterialSystem* MaterialSystem::GetInstance( )
{
	static MaterialSystem materialSystem;
	return &materialSystem;
}

void MaterialSystem::RegisterMaterial( const TCHAR* pName, std::unique_ptr<IMaterial> pMaterial )
{
	if ( pName && pMaterial )
	{
		auto found = m_materials.find( pName );

		if ( found == m_materials.end( ) )
		{
			m_materials.emplace( pName, std::move( pMaterial ) );
		}
	}
}

IMaterial* MaterialSystem::SearchMaterialByName( const TCHAR* pName )
{
	if ( !pName )
	{
		return nullptr;
	}

	auto found = m_materials.find( pName );

	if ( found != m_materials.end( ) )
	{
		return found->second.get();
	}

	return nullptr;
}

MaterialSystem::~MaterialSystem( )
{
	m_materials.clear( );
}