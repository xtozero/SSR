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
			pMaterial->SetConstantBuffers( &m_constantBuffers );
			m_materials.emplace( pName, std::move( pMaterial ) );
		}
	}
}

void MaterialSystem::RegisterConstantBuffer( UINT type, IBuffer* pConstantBuffer )
{
	if ( m_constantBuffers.size() > type && pConstantBuffer )
	{
		m_constantBuffers[type] = pConstantBuffer;
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
	//마테리얼 등록은 무조건 REGISTER_MATERIAL로 할것 안그러면 포인터가 해제되지 않음.
	m_materials.clear( );
}