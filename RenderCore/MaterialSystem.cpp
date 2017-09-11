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
	//���׸��� ����� ������ REGISTER_MATERIAL�� �Ұ� �ȱ׷��� �����Ͱ� �������� ����.
	m_materials.clear( );
}