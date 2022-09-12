#include "stdafx.h"
#include "GameObject/SkyBox.h"

#include "AssetLoader.h"
#include "Components/TexturedSkyComponent.h"
#include "Core/GameLogic.h"
#include "GameObject/GameObjectFactory.h"
#include "Json/json.hpp"
#include "Mesh/MeshDescription.h"
#include "Mesh/StaticMesh.h"

#include <numeric>

DECLARE_GAME_OBJECT( skybox, CSkyBox );

namespace
{
	constexpr float SKYBOX_LENGTH = 10.f;
}

CSkyBox::CSkyBox( )
{
	m_pTexturedSkyComponent = CreateComponent<TexturedSkyComponent>( *this, "TexturedSkyComponent" );
	auto pStaticMesh = std::make_shared<rendercore::StaticMesh>( );

	std::vector<rendercore::MeshDescription> descs( 1 );
	rendercore::MeshDescription& boxDesc = descs[0];

	boxDesc.m_positions.resize( 8 );
	boxDesc.m_positions[0] = Vector( -SKYBOX_LENGTH, -SKYBOX_LENGTH, -SKYBOX_LENGTH );
	boxDesc.m_positions[1] = Vector( -SKYBOX_LENGTH, -SKYBOX_LENGTH, SKYBOX_LENGTH );
	boxDesc.m_positions[2] = Vector( -SKYBOX_LENGTH, SKYBOX_LENGTH, -SKYBOX_LENGTH );
	boxDesc.m_positions[3] = Vector( -SKYBOX_LENGTH, SKYBOX_LENGTH, SKYBOX_LENGTH );
	boxDesc.m_positions[4] = Vector( SKYBOX_LENGTH, -SKYBOX_LENGTH, -SKYBOX_LENGTH );
	boxDesc.m_positions[5] = Vector( SKYBOX_LENGTH, -SKYBOX_LENGTH, SKYBOX_LENGTH );
	boxDesc.m_positions[6] = Vector( SKYBOX_LENGTH, SKYBOX_LENGTH, -SKYBOX_LENGTH );
	boxDesc.m_positions[7] = Vector( SKYBOX_LENGTH, SKYBOX_LENGTH, SKYBOX_LENGTH );

	for ( int32 i = 0; i < static_cast<int32>( boxDesc.m_positions.size( ) ); ++i )
	{
		boxDesc.m_vertexInstances.emplace_back( i, -1, -1 );
	}

	boxDesc.m_triangles.resize( 12 );
	boxDesc.m_triangles[0].m_vertexInstanceID[0] = 4; boxDesc.m_triangles[0].m_vertexInstanceID[1] = 6; boxDesc.m_triangles[0].m_vertexInstanceID[2] = 0;
	boxDesc.m_triangles[1].m_vertexInstanceID[0] = 6; boxDesc.m_triangles[1].m_vertexInstanceID[1] = 2; boxDesc.m_triangles[1].m_vertexInstanceID[2] = 0;
	boxDesc.m_triangles[2].m_vertexInstanceID[0] = 2; boxDesc.m_triangles[2].m_vertexInstanceID[1] = 3; boxDesc.m_triangles[2].m_vertexInstanceID[2] = 0;
	boxDesc.m_triangles[3].m_vertexInstanceID[0] = 3; boxDesc.m_triangles[3].m_vertexInstanceID[1] = 1; boxDesc.m_triangles[3].m_vertexInstanceID[2] = 0;
	boxDesc.m_triangles[4].m_vertexInstanceID[0] = 6; boxDesc.m_triangles[4].m_vertexInstanceID[1] = 7; boxDesc.m_triangles[4].m_vertexInstanceID[2] = 2;
	boxDesc.m_triangles[5].m_vertexInstanceID[0] = 7; boxDesc.m_triangles[5].m_vertexInstanceID[1] = 3; boxDesc.m_triangles[5].m_vertexInstanceID[2] = 2;
	boxDesc.m_triangles[6].m_vertexInstanceID[0] = 7; boxDesc.m_triangles[6].m_vertexInstanceID[1] = 6; boxDesc.m_triangles[6].m_vertexInstanceID[2] = 4;
	boxDesc.m_triangles[7].m_vertexInstanceID[0] = 5; boxDesc.m_triangles[7].m_vertexInstanceID[1] = 7; boxDesc.m_triangles[7].m_vertexInstanceID[2] = 4;
	boxDesc.m_triangles[8].m_vertexInstanceID[0] = 5; boxDesc.m_triangles[8].m_vertexInstanceID[1] = 4; boxDesc.m_triangles[8].m_vertexInstanceID[2] = 0;
	boxDesc.m_triangles[9].m_vertexInstanceID[0] = 1; boxDesc.m_triangles[9].m_vertexInstanceID[1] = 5; boxDesc.m_triangles[9].m_vertexInstanceID[2] = 0;
	boxDesc.m_triangles[10].m_vertexInstanceID[0] = 7; boxDesc.m_triangles[10].m_vertexInstanceID[1] = 5; boxDesc.m_triangles[10].m_vertexInstanceID[2] = 1;
	boxDesc.m_triangles[11].m_vertexInstanceID[0] = 3; boxDesc.m_triangles[11].m_vertexInstanceID[1] = 7; boxDesc.m_triangles[11].m_vertexInstanceID[2] = 1;

	boxDesc.m_polygons.resize( 1 );
	auto& triangleID = boxDesc.m_polygons[0].m_triangleID;
	triangleID.resize( 12 );
	std::iota( std::begin( triangleID ), std::end( triangleID ), 0 );

	boxDesc.m_polygonMaterialName.resize( 1 );

	pStaticMesh->BuildMeshFromMeshDescriptions( descs );
	pStaticMesh->PostLoad( );
	m_pTexturedSkyComponent->SetStaticMesh( pStaticMesh );
}
