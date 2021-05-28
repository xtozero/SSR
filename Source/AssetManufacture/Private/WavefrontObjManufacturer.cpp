#include "WavefrontObjManufacturer.h"

#include "Material/Material.h"
#include "Mesh/MeshDescription.h"
#include "Mesh/StaticMesh.h"
#include "WavefrontObjParser.hpp"

#include <numeric>

namespace fs = std::filesystem;

namespace
{
	StaticMesh CreateStaticMeshFromWavefrontObj( const Wavefront::ObjModel& model, const fs::path& assetsRootPath )
	{
		std::vector<MeshDescription> meshDescriptions;
		meshDescriptions.emplace_back( );
		MeshDescription& meshDescription = meshDescriptions.back( );

		auto& pos = meshDescription.m_positions;
		pos.reserve( model.m_vertices.size( ) );
		for ( const auto& v : model.m_vertices )
		{
			pos.emplace_back( std::get<0>( v ), std::get<1>( v ), std::get<2>( v ) );
		}

		auto& normal = meshDescription.m_normals;
		normal.reserve( model.m_normal.size( ) );
		for ( const auto& n : model.m_normal )
		{
			normal.emplace_back( std::get<0>( n ), std::get<1>( n ), std::get<2>( n ) );
		}

		auto& texcoord = meshDescription.m_texCoords;
		texcoord.reserve( model.m_texcoord.size( ) );
		for ( const auto& t : model.m_texcoord )
		{
			texcoord.emplace_back( std::get<0>( t ), std::get<1>( t ) );
		}
		
		auto faceTriFold = []( std::size_t init, const Wavefront::Face& face )
		{
			assert( face.m_vertices.size( ) == 3 );
			std::size_t numTri = face.m_vertices.size( ) / 3;
			return init + numTri;
		};

		auto meshTriFold = [faceTriFold]( std::size_t init, const Wavefront::ObjMesh& mesh )
		{
			std::size_t faceTriangle = std::accumulate( std::begin( mesh.m_face ), std::end( mesh.m_face ), std::size_t( 0 ), faceTriFold );

			return init + faceTriangle;
		};

		std::size_t totalTriangle = std::accumulate( std::begin( model.m_meshs ), std::end( model.m_meshs ), std::size_t( 0 ), meshTriFold );

		auto& triangles = meshDescription.m_triangles;
		triangles.reserve( totalTriangle );

		auto& vertexInstances = meshDescription.m_vertexInstances;
		vertexInstances.reserve( totalTriangle * 3 );
		std::map<MeshVertexInstance, std::size_t> viLut;

		for ( const auto& mesh : model.m_meshs )
		{
			if ( mesh.m_face.size( ) == 0 )
			{
				continue;
			}

			auto& polygons = meshDescription.m_polygons;
			polygons.emplace_back( );
			auto& curPolygon = polygons.back( );

			auto& polygonMaterial = meshDescription.m_polygonMaterialName;
			polygonMaterial.emplace_back( mesh.m_materialName );

			for ( const auto& face : mesh.m_face )
			{
				MeshTriangle triangle;

				std::size_t vertexSize = face.m_vertices.size( );
				// Only support triangle list
				assert( vertexSize == 3 );
				std::size_t normalSize = face.m_normals.size( );
				std::size_t texcoordSize = face.m_texcoords.size( );

				for ( std::size_t i = 0; i < vertexSize; ++i )
				{
					int posIdx = static_cast<int>( face.m_vertices[i] );
					int normalIdx = ( normalSize == 0 ) ? -1 : static_cast<int>( face.m_normals[i] );
					int texIdx = ( texcoordSize == 0 ) ? -1 : static_cast<int>( face.m_texcoords[i] );

					std::size_t vertexInstanceID = 0;
					MeshVertexInstance vi( posIdx, normalIdx, texIdx );
					auto found = viLut.find( vi );

					if ( found == viLut.end( ) )
					{
						vertexInstanceID = vertexInstances.size( );
						vertexInstances.emplace_back( vi );
						viLut.emplace( vi, vertexInstanceID );
					}
					else
					{
						vertexInstanceID = found->second;
					}
					
					triangle.m_vertexInstanceID[i] = vertexInstanceID;
				}

				curPolygon.m_triangleID.emplace_back( triangles.size( ) );
				triangles.emplace_back( triangle );
			}
		}

		StaticMesh staticMesh;
		std::set<std::string> uniqueMaterial;
		for ( const auto& mesh : model.m_meshs )
		{
			if ( mesh.m_face.size( ) == 0 )
			{
				continue;
			}

			auto found = uniqueMaterial.find( mesh.m_materialName );
			if ( found == uniqueMaterial.end( ) )
			{
				uniqueMaterial.emplace( mesh.m_materialName );

				std::string prefix = mesh.m_materialLibraryName.empty() ? "M_" : "M_" + mesh.m_materialLibraryName + "_";
				fs::path materialAsset( prefix + mesh.m_materialName + ".asset" );

				auto mat = std::make_shared<Material>( mesh.m_materialName.c_str( ) );
				mat->SetPath( assetsRootPath / materialAsset );
				staticMesh.AddMaterial( mat );
			}
		}

		staticMesh.BuildMeshFromMeshDescriptions( meshDescriptions );

		return staticMesh;
	}

	fs::path ConvertTextureAssetPath( const fs::path& mtlFileName, const fs::path& texPath, const fs::path& destPath )
	{
		fs::path texName = texPath.filename( );
		texName.replace_extension( ".asset" );

		for ( const auto& p : fs::recursive_directory_iterator( destPath ) )
		{
			if ( p.is_directory( ) )
			{
				if ( p.path( ).filename( ) == mtlFileName )
				{
					return "." / fs::relative( p.path( ), destPath.parent_path( ) ) / texName;
				}
			}
		}

		return {};
	}

	Material CreateMaterialFromWavefrontMaterial( const std::string& mtlFileName, const std::string& materialName, const Wavefront::ObjMaterial& material, const fs::path& destPath )
	{
		Material assetMaterial( materialName.c_str( ) );

		if ( material.m_ambient )
		{
			const auto& ambientValue = material.m_ambient.value( );
			CXMFLOAT4 ambient( std::get<0>( ambientValue ), std::get<1>( ambientValue ), std::get<2>( ambientValue ), 1.f );
			assetMaterial.AddProperty( "Ambient", ambient );
		}

		if ( material.m_diffuse )
		{
			const auto& diffuseValue = material.m_diffuse.value( );
			CXMFLOAT4 diffuse( std::get<0>( diffuseValue ), std::get<1>( diffuseValue ), std::get<2>( diffuseValue ), 1.f );
			assetMaterial.AddProperty( "Diffuse", diffuse );
		}
		
		if ( material.m_specular )
		{
			const auto& specularValue = material.m_specular.value( );
			CXMFLOAT4 specular( std::get<0>( specularValue ), std::get<1>( specularValue ), std::get<2>( specularValue ), 1.f );
			assetMaterial.AddProperty( "Specular", specular );
		}
		
		if ( material.m_ambientTex.empty( ) == false )
		{
			fs::path ambientTex = ConvertTextureAssetPath( mtlFileName, material.m_ambientTex, destPath );

			if ( ambientTex.has_relative_path( ) )
			{
				auto ambient = std::make_shared<Texture>( );
				ambient->SetPath( material.m_ambientTex );
				assetMaterial.AddProperty( "AmbientMap", ambient );
			}
		}

		if ( material.m_diffuseTex.empty( ) == false )
		{
			fs::path diffuseTex = ConvertTextureAssetPath( mtlFileName, material.m_diffuseTex, destPath );

			if ( diffuseTex.has_relative_path( ) )
			{
				auto diffuse = std::make_shared<Texture>( );
				diffuse->SetPath( material.m_diffuseTex );
				assetMaterial.AddProperty( "DiffuseMap", diffuse );
			}
		}

		if ( material.m_specularTex.empty( ) == false )
		{
			fs::path specularTex = ConvertTextureAssetPath( mtlFileName, material.m_specularTex, destPath );

			if ( specularTex.has_relative_path( ) )
			{
				auto specular = std::make_shared<Texture>( );
				specular->SetPath( material.m_specularTex );
				assetMaterial.AddProperty( "SpecularMap", specular );
			}
		}

		return assetMaterial;
	}
}

bool WavefrontObjManufacturer::IsSuitable( const std::filesystem::path& srcPath ) const
{
	return srcPath.extension() == fs::path( ".obj" );
}

std::optional<Products> WavefrontObjManufacturer::Manufacture( const std::filesystem::path& srcPath, const std::filesystem::path& destPath ) const
{
	if ( fs::exists( srcPath ) == false )
	{
		return { };
	}

	Wavefront::ObjModel model;
	Wavefront::ObjParser parser;
	if ( parser.Parse( srcPath, model ) == false )
	{
		return { };
	}

	fs::path destRootPath = "." / fs::relative( destPath, destPath.parent_path( ) );
	fs::path assetsRootPath = destRootPath / fs::path( "Material" );

	Archive ar;
	StaticMesh staticMesh = CreateStaticMeshFromWavefrontObj( model, assetsRootPath );
	staticMesh.Serialize( ar );

	Products products;
	products.emplace_back( srcPath.filename( ), std::move( ar ) );
	return products;
}

bool WavefrontMtlManufacturer::IsSuitable( const std::filesystem::path& srcPath ) const
{
	return srcPath.extension( ) == fs::path( ".mtl" );
}

std::optional<Products> WavefrontMtlManufacturer::Manufacture( const std::filesystem::path& srcPath, const std::filesystem::path& destPath ) const
{
	if ( fs::exists( srcPath ) == false )
	{
		return { };
	}

	Wavefront::ObjMaterialLibrary mtl;
	Wavefront::ObjMtlParser parser;
	if ( parser.Parse( srcPath, mtl ) == false )
	{
		return { };
	}

	const std::string& mtlFileName = srcPath.stem( ).generic_string( );

	Products products;
	for ( const auto& namedMaterial : mtl.m_materials )
	{
		Archive ar;
		const auto& materialName = namedMaterial.first;
		const auto& material = namedMaterial.second;
		Material assetMaterial = CreateMaterialFromWavefrontMaterial( mtlFileName, materialName, material, destPath );
		assetMaterial.Serialize( ar );

		fs::path assetMaterialFileName( "M_" + mtlFileName + "_" + materialName + ".asset" );
		products.emplace_back( assetMaterialFileName, std::move( ar ) );
	}

	return products;
}
