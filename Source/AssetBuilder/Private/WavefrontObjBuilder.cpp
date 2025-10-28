#include "WavefrontObjBuilder.h"

#include "AssetBuilderConfig.h"
#include "Material/Material.h"
#include "Math/Vector4.h"
#include "Mesh/MeshDescription.h"
#include "Mesh/StaticMesh.h"
#include "RawAsset.h"
#include "SizedTypes.h"
#include "WavefrontObjParser.hpp"

#include <DirectXMath.h>
#include <meshoptimizer.h>
#include <numeric>

namespace fs = std::filesystem;

namespace
{
	Wavefront::Vec3 CalcTriangleNormal( const Wavefront::Vec3& v0, const Wavefront::Vec3& v1, const Wavefront::Vec3& v2 )
	{
		Vector f0( std::get<0>( v0 ), std::get<1>( v0 ), std::get<2>( v0 ) );
		Vector f1( std::get<0>( v1 ), std::get<1>( v1 ), std::get<2>( v1 ) );
		Vector f2( std::get<0>( v2 ), std::get<1>( v2 ), std::get<2>( v2 ) );

		auto e0 = ( f1 - f0 ).GetNormalized();
		auto e1 = ( f2 - f0 ).GetNormalized();

		auto n = e0 ^ e1;
		n = n.GetNormalized();

		Vector normal = n;
		return { normal.x, normal.y, normal.z };
	}

	std::vector<Wavefront::Vec3> CreateSmoothNormal( const Wavefront::ObjModel& model )
	{
		std::vector<Wavefront::Vec3> normals( model.m_vertices.size() );

		for ( const auto& mesh : model.m_meshs )
		{
			for ( const auto& face : mesh.m_faces )
			{
				assert( face.m_vertices.size() > 2 );
				for ( int32 i = 0; i < face.m_vertices.size() - 2; ++i )
				{
					int32 indicies[3] = {
						face.m_vertices[0],
						face.m_vertices[i + 1],
						face.m_vertices[i + 2]
					};

					Wavefront::Vec3 normal = CalcTriangleNormal( model.m_vertices[indicies[0]], model.m_vertices[indicies[1]], model.m_vertices[indicies[2]] );

					for ( int32 index : indicies )
					{
						std::get<0>( normals[index] ) += std::get<0>( normal );
						std::get<1>( normals[index] ) += std::get<1>( normal );
						std::get<2>( normals[index] ) += std::get<2>( normal );
					}
				}
			}
		}

		// Normalize vector
		for ( auto& normal : normals )
		{
			Vector n( std::get<0>( normal ), std::get<1>( normal ), std::get<2>( normal ) );
			n = n.GetNormalized();
			normal = { n.x, n.y, n.z };
		}

		return normals;
	}

	uint32 FindOrCreateVertexInstance( std::vector<rendercore::MeshVertexInstance>& vertexInstances, std::map<rendercore::MeshVertexInstance, uint32>& viLut, int32 posIdx, int32 normalIdx, int32 texIdx )
	{
		uint32 vertexInstanceId = 0;
		rendercore::MeshVertexInstance vi( posIdx, normalIdx, texIdx );
		auto found = viLut.find( vi );

		if ( found == std::end( viLut ) )
		{
			vertexInstanceId = static_cast<uint32>( vertexInstances.size() );
			vertexInstances.emplace_back( vi );
			viLut.emplace( vi, vertexInstanceId );
		}
		else
		{
			vertexInstanceId = found->second;
		}

		return vertexInstanceId;
	}

	std::unique_ptr<rendercore::StaticMesh> CreateStaticMeshFromWavefrontObj( const Wavefront::ObjModel& model, const fs::path& parentPath )
	{
		std::vector<rendercore::MeshDescription> meshDescriptions;
		meshDescriptions.emplace_back();
		rendercore::MeshDescription& meshDescription = meshDescriptions.back();

		auto& pos = meshDescription.m_positions;
		pos.reserve( model.m_vertices.size() );
		for ( const auto& v : model.m_vertices )
		{
			pos.emplace_back( std::get<0>( v ), std::get<1>( v ), std::get<2>( v ) );
		}

		auto& normal = meshDescription.m_normals;
		normal.reserve( model.m_normal.size() );
		for ( const auto& n : model.m_normal )
		{
			normal.emplace_back( std::get<0>( n ), std::get<1>( n ), std::get<2>( n ) );
		}

		auto& texcoord = meshDescription.m_texCoords;
		texcoord.reserve( model.m_texcoord.size() );
		for ( const auto& t : model.m_texcoord )
		{
			texcoord.emplace_back( std::get<0>( t ), 1.f - std::get<1>( t ) );
		}

		auto faceTriFold = []( size_t init, const Wavefront::Face& face )
		{
			assert( face.m_vertices.size() > 2 );
			size_t numTri = face.m_vertices.size() - 2;
			return init + numTri;
		};

		auto meshTriFold = [faceTriFold]( size_t init, const Wavefront::ObjMesh& mesh )
		{
			size_t faceTriangle = std::accumulate( std::begin( mesh.m_faces ), std::end( mesh.m_faces ), size_t( 0 ), faceTriFold );

			return init + faceTriangle;
		};

		size_t totalTriangle = std::accumulate( std::begin( model.m_meshs ), std::end( model.m_meshs ), size_t( 0 ), meshTriFold );

		auto& triangles = meshDescription.m_triangles;
		triangles.reserve( totalTriangle );

		auto& vertexInstances = meshDescription.m_vertexInstances;
		vertexInstances.reserve( totalTriangle * 3 );
		std::map<rendercore::MeshVertexInstance, uint32> viLut;

		for ( const auto& mesh : model.m_meshs )
		{
			if ( mesh.m_faces.size() == 0 )
			{
				continue;
			}

			auto& polygons = meshDescription.m_polygons;
			polygons.emplace_back();
			auto& curPolygon = polygons.back();

			auto& polygonMaterial = meshDescription.m_polygonMaterialName;
			polygonMaterial.emplace_back( Name( mesh.m_materialName ) );

			for ( const auto& face : mesh.m_faces )
			{
				size_t vertexSize = face.m_vertices.size();
				assert( vertexSize > 0 );
				size_t normalSize = face.m_normals.size();
				size_t texcoordSize = face.m_texcoords.size();

				uint32 firstVertexInstanceId = FindOrCreateVertexInstance(
					vertexInstances,
					viLut,
					face.m_vertices[0],
					( normalSize == 0 ) ? -1 : face.m_normals[0],
					( texcoordSize == 0 ) ? -1 : face.m_texcoords[0] );

				for ( size_t i = 0; i < vertexSize - 2; ++i )
				{
					uint32 secondVertexInstanceId = FindOrCreateVertexInstance(
						vertexInstances,
						viLut,
						face.m_vertices[i + 1],
						( normalSize == 0 ) ? -1 : face.m_normals[i + 1],
						( texcoordSize == 0 ) ? -1 : face.m_texcoords[i + 1] );

					uint32 thirdVertexInstanceId = FindOrCreateVertexInstance(
						vertexInstances,
						viLut,
						face.m_vertices[i + 2],
						( normalSize == 0 ) ? -1 : face.m_normals[i + 2],
						( texcoordSize == 0 ) ? -1 : face.m_texcoords[i + 2] );

					rendercore::MeshTriangle triangle = {
						firstVertexInstanceId,
						secondVertexInstanceId,
						thirdVertexInstanceId
					};

					curPolygon.m_triangleId.emplace_back( static_cast<uint32>( triangles.size() ) );
					triangles.emplace_back( triangle );
				}
			}
		}

		// Build Meshlet
		{
			std::vector<Vector> vertices;
			vertices.reserve( meshDescription.m_vertexInstances.size() );

			for ( const rendercore::MeshVertexInstance& vertexInstance : meshDescription.m_vertexInstances )
			{
				vertices.emplace_back( pos[vertexInstance.m_positionId] );
			}

			std::vector<uint32> meshletVertices;
			meshletVertices.reserve( meshDescription.m_triangles.size() * 3 );

			std::vector<uint32> meshletTriangles;
			meshletTriangles.reserve( meshDescription.m_triangles.size() * 3 );

			for ( rendercore::MeshPolygon& polygon : meshDescription.m_polygons )
			{
				std::vector<uint32> indices;
				indices.reserve( polygon.m_triangleId.size() * 3 );

				for ( uint32 triangleId : polygon.m_triangleId )
				{
					const rendercore::MeshTriangle& triangle = meshDescription.m_triangles[triangleId];

					for ( uint32 vertexInstanceId : triangle.m_vertexInstanceId )
					{
						indices.emplace_back( vertexInstanceId );
					}
				}

				constexpr size_t MaxVertices = 64;
				constexpr size_t MaxTriangles = 124;
				constexpr float ConeWeight = 0.f;

				size_t maxMashlets = meshopt_buildMeshletsBound( indices.size(), MaxVertices, MaxTriangles );

				std::vector<meshopt_Meshlet> meshlets( maxMashlets );
				std::vector<uint32> polygonVertices( maxMashlets * MaxVertices );
				std::vector<uint8> polygonTriangles( maxMashlets * MaxTriangles * 3 );

				size_t meshletCount = meshopt_buildMeshlets( meshlets.data(), polygonVertices.data(), polygonTriangles.data(), indices.data(), indices.size(), &vertices[0].x, vertices.size(), sizeof( Vector ), MaxVertices, MaxTriangles, ConeWeight );

				meshlets.resize( meshletCount );

				polygon.m_meshlets.reserve( meshlets.size() );

				const meshopt_Meshlet& lastMeshlet = meshlets.back();

				uint32 numVertices = lastMeshlet.vertex_offset + lastMeshlet.vertex_count;
				
				auto vertexOffset = static_cast<uint32>( meshletVertices.size() );

				for ( const meshopt_Meshlet& meshlet : meshlets )
				{
					auto triangleOffset = static_cast<uint32>( meshletTriangles.size() );

					for ( uint32 i = 0; i < meshlet.triangle_count; ++i )
					{
						uint32 i0 = 3 * i + meshlet.triangle_offset;
						uint32 i1 = 3 * i + 1 + meshlet.triangle_offset;
						uint32 i2 = 3 * i + 2 + meshlet.triangle_offset;

						uint8 idx0 = polygonTriangles[i0];
						uint8 idx1 = polygonTriangles[i1];
						uint8 idx2 = polygonTriangles[i2];
						uint32 packed = ( idx2 << 16 ) | ( idx1 << 8 ) | idx0;

						meshletTriangles.emplace_back( packed );
					}
					
					meshopt_Bounds meshletBounds = meshopt_computeMeshletBounds( &polygonVertices[meshlet.vertex_offset], &polygonTriangles[meshlet.triangle_offset], meshlet.triangle_count, &vertices[0].x, vertices.size(), sizeof( Vector ) );

					uint32 meshletVertexOffset = meshlet.vertex_offset + vertexOffset;
					polygon.m_meshlets.emplace_back( meshletVertexOffset, triangleOffset, meshlet.vertex_count, meshlet.triangle_count, Vector( meshletBounds.center ), meshletBounds.radius);
				}

				meshletVertices.insert( std::end( meshletVertices ), &polygonVertices[0], &polygonVertices[numVertices] );
			}

			meshDescription.m_meshletVertices = std::move( meshletVertices );
			meshDescription.m_meshletTriangles = std::move( meshletTriangles );
		}

		auto staticMesh = std::make_unique<rendercore::StaticMesh>();
		std::set<std::string> uniqueMaterial;
		for ( const auto& mesh : model.m_meshs )
		{
			if ( mesh.m_faces.size() == 0 )
			{
				continue;
			}

			if ( uniqueMaterial.contains( mesh.m_materialName ) == false )
			{
				uniqueMaterial.emplace( mesh.m_materialName );

				std::string prefix = mesh.m_materialLibraryName.empty() ? "M_" : "M_" + mesh.m_materialLibraryName + "_";
				fs::path materialAsset( prefix + mesh.m_materialName + ".asset" );

				auto mat = std::make_shared<rendercore::Material>( mesh.m_materialName.c_str() );
				mat->SetPath( parentPath / materialAsset );
				staticMesh->AddMaterial( mat );
			}
		}

		staticMesh->BuildMeshFromMeshDescriptions( meshDescriptions );
		staticMesh->Bounds() = BoxSphereBounds( pos.data(), static_cast<uint32>( pos.size() ) );

		return staticMesh;
	}

	fs::path ConvertTextureAssetPath( const fs::path& mtlFileName, const fs::path& texPath, const fs::path& destPath )
	{
		fs::path texName = texPath.filename();
		texName.replace_extension( ".asset" );

		for ( const auto& p : fs::recursive_directory_iterator( destPath ) )
		{
			if ( p.is_directory() )
			{
				if ( p.path().filename() == mtlFileName )
				{
					return "." / fs::relative( p.path(), destPath ) / texName;
				}
			}
		}

		return {};
	}

	json::Value ConvertWavefrontMtlToJsonMaterial( const std::string& mtlFileName, const Wavefront::ObjMaterial& material )
	{
		json::Value root( json::DataType::Object );
		root["Type"] = "Material";
		root["Shader"] = json::Value( json::DataType::Object );
		root["Shader"]["VS"] = "./Assets/Shaders/Material/VS_DefaultLit.asset";
		root["Shader"]["PS"] = "./Assets/Shaders/Material/PS_DefaultLit.asset";
		root["Shader"]["CS"] = "./Assets/Shaders/Material/CS_DefaultLit.asset";
		root["Surface"] = json::Value( json::DataType::Object );

		json::Value& surface = root["Surface"];

		surface["Alpha"] = material.m_alpha;
		surface["Roughness"] = 1.f;
		surface["IndexOfRefraction"] = material.m_ior;
		surface["SpecularExponent"] = material.m_specularExponent;

		if ( material.m_ambient )
		{
			const auto& ambientValue = material.m_ambient.value();
			float ambient[] = { 
				std::get<0>( ambientValue ),
				std::get<1>( ambientValue ),
				std::get<2>( ambientValue ) 
			};

			surface["Ambient"] = json::Value( json::DataType::Array );
			for ( int32 i = 0; i < 3; ++i )
			{
				surface["Ambient"][i] = ambient[i];
			}
		}

		if ( material.m_diffuse )
		{
			const auto& diffuseValue = material.m_diffuse.value();
			float diffuse[] = { 
				std::get<0>( diffuseValue ),
				std::get<1>( diffuseValue ), 
				std::get<2>( diffuseValue ) 
			};

			surface["Diffuse"] = json::Value( json::DataType::Array );
			for ( int32 i = 0; i < 3; ++i )
			{
				surface["Diffuse"][i] = diffuse[i];
			}
		}

		if ( material.m_specular )
		{
			const auto& specularValue = material.m_specular.value();
			float specular[] = { 
				std::get<0>( specularValue ), 
				std::get<1>( specularValue ), 
				std::get<2>( specularValue )
			};

			surface["Specular"] = json::Value( json::DataType::Array );
			for ( int32 i = 0; i < 3; ++i )
			{
				surface["Specular"][i] = specular[i];
			}
		}

		if ( material.m_transmission )
		{
			const auto& transmissionValue = material.m_transmission.value();
			float transmission[] = {
				std::get<0>( transmissionValue ),
				std::get<1>( transmissionValue ),
				std::get<2>( transmissionValue )
			};

			surface["Transmission"] = json::Value( json::DataType::Array );
			for ( int32 i = 0; i < 3; ++i )
			{
				surface["Transmission"][i] = transmission[i];
			}
		}

		if ( material.m_ambientTex.empty() == false )
		{
			fs::path ambientTex = ConvertTextureAssetPath( mtlFileName, material.m_ambientTex, AssetBuilderConfig::Instance().RootDirectory() );

			if ( ambientTex.has_relative_path() )
			{
				surface["AmbientTexture"] = ambientTex.generic_string();
				surface["AmbientTextureSampler"] = "./Assets/RenderOptions/SO_Default.asset";
			}
		}

		if ( material.m_diffuseTex.empty() == false )
		{
			fs::path diffuseTex = ConvertTextureAssetPath( mtlFileName, material.m_diffuseTex, AssetBuilderConfig::Instance().RootDirectory() );

			if ( diffuseTex.has_relative_path() )
			{
				surface["DiffuseTexture"] = diffuseTex.generic_string();
				surface["DiffuseTextureSampler"] = "./Assets/RenderOptions/SO_Default.asset";
			}
		}

		if ( material.m_specularTex.empty() == false )
		{
			fs::path specularTex = ConvertTextureAssetPath( mtlFileName, material.m_specularTex, AssetBuilderConfig::Instance().RootDirectory() );

			if ( specularTex.has_relative_path() )
			{
				surface["SpecularTexture"] = specularTex.generic_string();
				surface["SpecularTextureSampler"] = "./Assets/RenderOptions/SO_Default.asset";
			}
		}

		if ( material.m_bumpTex.empty() == false )
		{
			fs::path normalTex = ConvertTextureAssetPath( mtlFileName, material.m_bumpTex, AssetBuilderConfig::Instance().RootDirectory() );

			if ( normalTex.has_relative_path() )
			{
				surface["NormalTexture"] = normalTex.generic_string();
				surface["NormalTextureSampler"] = "./Assets/RenderOptions/SO_Default.asset";
			}
		}

		return root;
	}
}

bool WavefrontObjBuilder::IsSuitable( const std::filesystem::path& srcPath ) const
{
	fs::path extension = ToLower( srcPath.extension().generic_string() );
	return extension == fs::path( ".obj" );
}

std::optional<Products> WavefrontObjBuilder::Build( const PathEnvironment& env, const std::filesystem::path& path ) const
{
	if ( fs::exists( path ) == false )
	{
		return { };
	}

	Wavefront::ObjModel model;
	Wavefront::ObjParser parser;
	if ( parser.Parse( path, model ) == false )
	{
		return { };
	}

	if ( model.m_normal.empty() )
	{
		model.m_normal = CreateSmoothNormal( model );
		for ( auto& mesh : model.m_meshs )
		{
			for ( auto& face : mesh.m_faces )
			{
				face.m_normals = face.m_vertices;
			}
		}
	}

	fs::path destParentPath = env.m_destination / fs::relative( path.parent_path() );
	destParentPath = "." / fs::relative( destParentPath, AssetBuilderConfig::Instance().RootDirectory() );

	auto staticMesh = CreateStaticMeshFromWavefrontObj( model, destParentPath );

	Products products;
	products.emplace_back( path.filename(), std::move( staticMesh ) );
	return products;
}

bool WavefrontMtlBuilder::IsSuitable( const std::filesystem::path& srcPath ) const
{
	fs::path extension = ToLower( srcPath.extension().generic_string() );
	return extension == fs::path( ".mtl" );
}

std::optional<Products> WavefrontMtlBuilder::Build( [[maybe_unused]] const PathEnvironment& env, const std::filesystem::path& path ) const
{
	if ( fs::exists( path ) == false )
	{
		return { };
	}

	Wavefront::ObjMaterialLibrary mtl;
	Wavefront::ObjMtlParser parser;
	if ( parser.Parse( path, mtl ) == false )
	{
		return { };
	}

	const std::string& mtlFileName = path.stem().generic_string();

	Products products;
	for ( const auto& namedMaterial : mtl.m_materials )
	{
		const auto& materialName = namedMaterial.first;
		const auto& material = namedMaterial.second;
		json::Value materialJson = ConvertWavefrontMtlToJsonMaterial( mtlFileName, material );
		std::string jsonStr = json::Writer::ToStringPretty( materialJson );

		BinaryChunk jsonData( static_cast<uint32>( jsonStr.size() ) );
		std::memcpy( jsonData.Data(), jsonStr.c_str(), jsonStr.size() );

		auto jsonAsset = std::make_unique<RawAsset>( jsonData );

		fs::path assetMaterialFileName( "M_" + mtlFileName + "_" + materialName + ".json" );
		products.emplace_back( assetMaterialFileName, std::move( jsonAsset ) );
	}

	return products;
}
