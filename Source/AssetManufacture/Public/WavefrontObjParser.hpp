#pragma once

#include "SizedTypes.h"

#include <cctype>
#include <fstream>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include <vector>

namespace Wavefront
{
	using Vec2 = std::tuple<float, float>;
	using Vec3 = std::tuple<float, float, float>;
	using Color = std::tuple<float, float, float>;
	using IndexVector = std::vector<int32>;

	struct Face
	{
		IndexVector m_vertices;
		IndexVector m_normals;
		IndexVector m_texcoords;
	};

	struct Object
	{
		std::string m_name;
		IndexVector m_mesh;
	};

	struct ObjMesh
	{
		std::vector<Face> m_faces;
		std::string m_materialLibraryName;
		std::string m_materialName;
	};

	struct ObjModel
	{
		std::map<std::string, Object> m_objects;

		std::vector<ObjMesh> m_meshs;

		std::vector<Vec3> m_vertices;
		std::vector<Vec3> m_normal;
		std::vector<Vec3> m_texcoord;
	};

	class ParserBase
	{
	protected:
		void Parse( const char* contents, size_t length )
		{
			m_begin = contents;
			m_end = contents + length;
			m_current = contents;
		}

		bool CanRead()
		{
			return m_current < m_end;
		}

		void SkipBlank()
		{
			while ( CanRead() )
			{
				char c = *m_current;
				if ( c == ' ' || c == '\t' )
				{
					++m_current;
				}
				else
				{
					break;
				}
			}
		}

		void SkipWhiteSpace()
		{
			while ( CanRead() )
			{
				char c = *m_current;
				if ( c == ' ' || c == '\t' || c == '\n' || c == '\r' )
				{
					++m_current;
				}
				else
				{
					break;
				}
			}
		}

		void SkipLine()
		{
			while ( CanRead() )
			{
				char c = *m_current;
				if ( c == '\n' )
				{
					break;
				}
				else
				{
					++m_current;
				}
			}
		}

		bool IsLineEnd()
		{
			char c = *m_current;
			return c == '\n' || c == '\r';
		}

		char GetNextChar()
		{
			return CanRead() ? *m_current++ : 0;
		}

		char PeekNextChar()
		{
			return CanRead() ? *m_current : 0;
		}

		bool MatchNextString( const char* str, size_t length )
		{
			if ( ( m_current > m_end ) || ( static_cast<size_t>( m_end - m_current ) < length ) )
			{
				return false;
			}

			if ( std::strncmp( m_current, str, length ) )
			{
				return false;
			}

			m_current += length;
			return true;
		}

		std::string ReadWord()
		{
			const char* begin = m_current;

			while ( CanRead() )
			{
				char c = *m_current;
				if ( c != ' ' && c != '\t' && c != '\n' && c != '\r' )
				{
					++m_current;
				}
				else
				{
					break;
				}
			}

			return std::string( begin, m_current );
		}

		float ReadFloat()
		{
			SkipWhiteSpace();
			return static_cast<float>( std::atof( ReadWord().c_str() ) );
		}

		int32 ReadInt()
		{
			SkipWhiteSpace();
			return std::atoi( ReadWord().c_str() );
		}

		const char* m_begin = nullptr;
		const char* m_end = nullptr;
		const char* m_current = nullptr;
	};

	struct ObjMaterial
	{
		std::optional<Color> m_ambient;
		std::optional<Color> m_diffuse;
		std::optional<Color> m_emissive;
		std::optional<Color> m_specular;
		std::optional<Color> m_transmission;

		float m_specularExponent = 0.f;
		float m_ior = 1.f;
		float m_alpha = 1.f;

		int32 m_illuminationModel = 1;

		std::string m_ambientTex;
		std::string m_diffuseTex;
		std::string m_specularTex;
		std::string m_bumpTex;
	};

	struct ObjMaterialLibrary
	{
		std::map<std::string, ObjMaterial> m_materials;
	};

	class ObjMtlParser final : public ParserBase
	{
	public:
		bool Parse( std::filesystem::path filePath, ObjMaterialLibrary& mtl )
		{
			if ( std::filesystem::exists( filePath ) == false )
			{
				return false;
			}

			std::ifstream objFile( filePath );

			if ( objFile.good() )
			{
				return Parse( objFile, mtl );
			}

			return false;
		}

		bool Parse( std::ifstream& file, ObjMaterialLibrary& mtl )
		{
			std::string contents;
			std::getline( file, contents, static_cast<char>( EOF ) );

			return Parse( contents.data(), contents.size(), mtl );
		}

		bool Parse( const char* contents, size_t length, ObjMaterialLibrary& mtl )
		{
			ParserBase::Parse( contents, length );
			return ParseMtl( mtl );
		}

	private:
		void CreateMaterial( ObjMaterialLibrary& mtl )
		{
			SkipWhiteSpace();
			std::string name = ReadWord();
			auto result = mtl.m_materials.emplace( name, ObjMaterial() );
			m_curMaterial = &result.first->second;
		}

		void ReadRGB( Vec3& rgb )
		{
			SkipWhiteSpace();
			std::string word = ReadWord();
			float r = static_cast<float>( atof( word.c_str() ) );

			float g = r;
			float b = r;

			// g and b are optional
			if ( IsLineEnd() == false )
			{
				// read g
				SkipWhiteSpace();
				word = ReadWord();
				g = static_cast<float>( atof( word.c_str() ) );

				// read b
				SkipWhiteSpace();
				word = ReadWord();
				b = static_cast<float>( atof( word.c_str() ) );
			}

			std::get<0>( rgb ) = r;
			std::get<1>( rgb ) = g;
			std::get<2>( rgb ) = b;
		}

		enum class TokenType : uint8
		{
			Error = 0,
			Comment,			// #
			NewMtl,				// newmtl
			Ambient,			// Ka
			Diffuse,			// Kd
			Emissive,			// Ke
			Specular,			// Ks
			SpecularExponent,	// Ns
			IndexOfRefraction,	// Ni
			Alpha,				// d
			Transmission,		// Tf
			Transparent,		// Tr
			IllumType,			// illum
			AmbientTexture,		// map_Ka
			DiffuseTexture,		// map_Kd
			SpecularTexture,	// map_Ks
			BumpTexture,		// map_bump or bump
		};

		struct Token
		{
			TokenType m_type = TokenType::Error;
			const char* m_begin = nullptr;
			const char* m_end = nullptr;
		};

		Token ReadToken()
		{
			SkipWhiteSpace();

			Token token;
			token.m_type = TokenType::Error;
			token.m_begin = m_current;

			switch ( GetNextChar() )
			{
			case '#':
				token.m_type = TokenType::Comment;
				break;
			case 'K':
				if ( MatchNextString( "a", std::strlen( "a" ) ) )
				{
					token.m_type = TokenType::Ambient;
				}
				else if ( MatchNextString( "d", std::strlen( "d" ) ) )
				{
					token.m_type = TokenType::Diffuse;
				}
				else if ( MatchNextString( "e", std::strlen( "e" ) ) )
				{
					token.m_type = TokenType::Emissive;
				}
				else if ( MatchNextString( "s", std::strlen( "s" ) ) )
				{
					token.m_type = TokenType::Specular;
				}
				break;
			case 'N':
				if ( MatchNextString( "s", std::strlen( "s" ) ) )
				{
					token.m_type = TokenType::SpecularExponent;
				}
				else if ( MatchNextString( "i", std::strlen( "i" ) ) )
				{
					token.m_type = TokenType::IndexOfRefraction;
				}
				break;
			case 'T':
				if ( MatchNextString( "f", std::strlen( "f" ) ) )
				{
					token.m_type = TokenType::Transmission;
				}
				else if ( MatchNextString( "r", std::strlen( "r" ) ) )
				{
					token.m_type = TokenType::Transparent;
				}
				break;
			case 'b':
				if ( MatchNextString( "ump", std::strlen( "ump" ) ) )
				{
					token.m_type = TokenType::BumpTexture;
				}
				break;
			case 'd':
				if ( PeekNextChar() == ' ' )
				{
					token.m_type = TokenType::Alpha;
				}
				break;
			case 'i':
				if ( MatchNextString( "llum", std::strlen( "llum" ) ) )
				{
					token.m_type = TokenType::IllumType;
				}
				break;
			case 'm':
				if ( MatchNextString( "ap_Ka", std::strlen( "ap_Ka" ) ) )
				{
					token.m_type = TokenType::AmbientTexture;
				}
				else if ( MatchNextString( "ap_Kd", std::strlen( "ap_Kd" ) ) )
				{
					token.m_type = TokenType::DiffuseTexture;
				}
				else if ( MatchNextString( "ap_Ks", std::strlen( "ap_Ks" ) ) )
				{
					token.m_type = TokenType::SpecularTexture;
				}
				else if ( MatchNextString( "ap_bump", std::strlen( "ap_bump" ) ) )
				{
					token.m_type = TokenType::BumpTexture;
				}
				break;
			case 'n':
				if ( MatchNextString( "ewmtl", std::strlen( "ewmtl" ) ) )
				{
					token.m_type = TokenType::NewMtl;
				}
				break;
			default:
				break;
			}

			token.m_end = m_current;
			return token;
		}

		bool ParseMtl( ObjMaterialLibrary& mtl )
		{
			while ( CanRead() )
			{
				Token token = ReadToken();

				switch ( token.m_type )
				{
				case TokenType::Error:
					if ( CanRead() )
					{
						return false;
					}
					break;
				case TokenType::Comment:
					SkipLine();
					break;
				case TokenType::NewMtl:
					CreateMaterial( mtl );
					break;
				case TokenType::Ambient:
				{
					Color ambient;
					ReadRGB( ambient );
					m_curMaterial->m_ambient = ambient;
				}
				break;
				case TokenType::Diffuse:
				{
					Color diffuse;
					ReadRGB( diffuse );
					m_curMaterial->m_diffuse = diffuse;
				}
				break;
				case TokenType::Emissive:
				{
					Color emissive;
					ReadRGB( emissive );
					m_curMaterial->m_emissive = emissive;
				}
				break;
				case TokenType::Specular:
				{
					Color pecular;
					ReadRGB( pecular );
					m_curMaterial->m_specular = pecular;
				}
				break;
				case TokenType::SpecularExponent:
					m_curMaterial->m_specularExponent = ReadFloat();
					break;
				case TokenType::IndexOfRefraction:
					m_curMaterial->m_ior = ReadFloat();
					break;
				case TokenType::Alpha:
					m_curMaterial->m_alpha = ReadFloat();
					break;
				case TokenType::Transmission:
				{
					Color transmission;
					ReadRGB( transmission );
					m_curMaterial->m_transmission = transmission;
				}
				break;
				case TokenType::Transparent:
				{
					float transparency = ReadFloat();
					m_curMaterial->m_alpha = 1.f - transparency;
				}
				break;
				case TokenType::IllumType:
					m_curMaterial->m_illuminationModel = ReadInt();
					break;
				case TokenType::AmbientTexture:
					SkipWhiteSpace();
					m_curMaterial->m_ambientTex = ReadWord();
					break;
				case TokenType::DiffuseTexture:
					SkipWhiteSpace();
					m_curMaterial->m_diffuseTex = ReadWord();
					break;
				case TokenType::SpecularTexture:
					SkipWhiteSpace();
					m_curMaterial->m_specularTex = ReadWord();
					break;
				case TokenType::BumpTexture:
					SkipWhiteSpace();
					m_curMaterial->m_bumpTex = ReadWord();
				}
			}

			return true;
		}

		ObjMaterial* m_curMaterial = nullptr;
	};

	class ObjParser final : public ParserBase
	{
	public:
		bool Parse( std::filesystem::path filePath, ObjModel& mesh )
		{
			if ( std::filesystem::exists( filePath ) == false )
			{
				return false;
			}

			std::ifstream objFile( filePath );

			if ( objFile.good() )
			{
				m_workingPath = filePath.parent_path();
				return Parse( objFile, mesh );
			}

			return false;
		}

		bool Parse( std::ifstream& file, ObjModel& mesh )
		{
			std::string contents;
			std::getline( file, contents, static_cast<char>( EOF ) );

			return Parse( contents.data(), contents.size(), mesh );
		}

		bool Parse( const char* contents, size_t length, ObjModel& mesh )
		{
			ParserBase::Parse( contents, length );
			return ParseObj( mesh );
		}

	private:
		static constexpr const char* DefaultObjectName = "default";
		static constexpr const char* DefaultMaterialName = "default";
		void CreateMesh( ObjModel& mesh )
		{
			if ( m_curObject == nullptr )
			{
				CreateObject( DefaultObjectName, mesh );
			}

			m_curObject->m_mesh.push_back( static_cast<int32>( mesh.m_meshs.size() ) );
			mesh.m_meshs.emplace_back();
			m_curMesh = &mesh.m_meshs.back();
			m_curMesh->m_materialName = DefaultMaterialName;
		}

		void CreateObject( const std::string& name, ObjModel& mesh )
		{
			auto& objects = mesh.m_objects;

			auto found = objects.find( name );
			if ( found == std::end( objects ) )
			{
				objects.emplace( std::pair( name, Object{} ) );
				found = objects.find( name );

				Object& object = found->second;
				object.m_name = name;
			}

			Object& object = found->second;
			m_curObject = &object;

			CreateMesh( mesh );
		}

		enum class TokenType : uint8
		{
			Error = 0,
			Comment,			// #
			Face,				// f
			Group,				// g
			Object,				// o
			SmoothingGroup,		// s
			Vertex,				// v
			TextureCoord,		// vt
			Normal,				// vn
			Number,
			UseMaterial,		// usemtl
			IncludeMaterial,	// mtllib
		};

		struct Token
		{
			TokenType m_type = TokenType::Error;
			const char* m_begin = nullptr;
			const char* m_end = nullptr;
		};

		Token ReadToken()
		{
			SkipWhiteSpace();

			Token token;
			token.m_type = TokenType::Error;
			token.m_begin = m_current;

			switch ( GetNextChar() )
			{
			case '#':
				token.m_type = TokenType::Comment;
				break;
			case '0':
				[[fallthrough]];
			case '1':
				[[fallthrough]];
			case '2':
				[[fallthrough]];
			case '3':
				[[fallthrough]];
			case '4':
				[[fallthrough]];
			case '5':
				[[fallthrough]];
			case '6':
				[[fallthrough]];
			case '7':
				[[fallthrough]];
			case '8':
				[[fallthrough]];
			case '9':
				[[fallthrough]];
			case '-':
				token.m_type = TokenType::Number;
				ReadWord();
				break;
			case 'f':
				token.m_type = TokenType::Face;
				break;
			case 'g':
				token.m_type = TokenType::Group;
				break;
			case 'm':
				if ( MatchNextString( "tllib", std::strlen( "tllib" ) ) )
				{
					token.m_type = TokenType::IncludeMaterial;
				}
				break;
			case 'o':
				token.m_type = TokenType::Object;
				break;
			case 's':
				token.m_type = TokenType::SmoothingGroup;
				break;
			case 'v':
				if ( MatchNextString( "t", 1 ) )
				{
					token.m_type = TokenType::TextureCoord;
				}
				else if ( MatchNextString( "n", 1 ) )
				{
					token.m_type = TokenType::Normal;
				}
				else
				{
					token.m_type = TokenType::Vertex;
				}
				break;
			case 'u':
				if ( MatchNextString( "semtl", std::strlen( "semtl" ) ) )
				{
					token.m_type = TokenType::UseMaterial;
				}
				break;
			default:
				break;
			}

			token.m_end = m_current;
			return token;
		}

		void CopyTokenContent( const Token& token, char* buffer, size_t size )
		{
			size_t contentSize = token.m_end - token.m_begin;
			contentSize = ( contentSize > ( size - 1 ) ) ? ( size - 1 ) : contentSize;
			std::memcpy( buffer, token.m_begin, contentSize );
			buffer[contentSize] = '\0';
		}

		void ReadVec3( std::vector<Vec3>& vec )
		{
			char buffer[1024];

			float x = 0.f;
			Token token = ReadToken();
			if ( token.m_type == TokenType::Number )
			{
				CopyTokenContent( token, buffer, std::extent_v<decltype( buffer )> );
				x = static_cast<float>( std::atof( buffer ) );
			}

			float y = 0.f;
			token = ReadToken();
			if ( token.m_type == TokenType::Number )
			{
				CopyTokenContent( token, buffer, std::extent_v<decltype( buffer )> );
				y = static_cast<float>( std::atof( buffer ) );
			}

			float z = 0.f;
			token = ReadToken();
			if ( token.m_type == TokenType::Number )
			{
				CopyTokenContent( token, buffer, std::extent_v<decltype( buffer )> );
				z = static_cast<float>( std::atof( buffer ) );
			}

			vec.emplace_back( x, y, z );
		}

		void ReadTexcoord( std::vector<Vec3>& vec )
		{
			char temp[1024];
			float u = 0.f;
			float v = 0.f;
			float w = 0.f;

			Token token = ReadToken();
			if ( token.m_type == TokenType::Number )
			{
				CopyTokenContent( token, temp, std::extent_v<decltype( temp )> );
				u = static_cast<float>( std::atof( temp ) );
			}

			// v is an optional argument
			const char* prevPos = m_current; // save current
			token = ReadToken();
			if ( token.m_type == TokenType::Number )
			{
				CopyTokenContent( token, temp, std::extent_v<decltype( temp )> );
				v = static_cast<float>( std::atof( temp ) );

				// w is an optional argument
				prevPos = m_current; // save current
				token = ReadToken();
				if ( token.m_type == TokenType::Number )
				{
					CopyTokenContent( token, temp, std::extent_v<decltype( temp )> );
					w = static_cast<float>( std::atof( temp ) );
				}
				else
				{
					m_current = prevPos;
				}
			}
			else
			{
				m_current = prevPos;
			}

			vec.emplace_back( u, v, w );
		}

		void ReadFace( ObjModel& mesh )
		{
			if ( m_curMesh == nullptr )
			{
				CreateObject( DefaultObjectName, mesh );
			}

			m_curMesh->m_faces.emplace_back();
			Face& face = m_curMesh->m_faces.back();

			int32 vSize = static_cast<int32>( mesh.m_vertices.size() );
			int32 vtSize = static_cast<int32>( mesh.m_texcoord.size() );
			int32 vnSize = static_cast<int32>( mesh.m_normal.size() );

			int32 col = 0;
			while ( CanRead() )
			{
				SkipBlank();
				if ( IsLineEnd() )
				{
					break;
				}

				char nextChar = PeekNextChar();
				if ( nextChar == '/' )
				{
					++col;
					++m_current;
					nextChar = PeekNextChar();
				}
				else
				{
					col = 0;
				}

				int32 val = 1;
				if ( nextChar == '-' )
				{
					val = -1;
					++m_current;
				}

				if ( std::isdigit( *m_current ) == 0 )
				{
					continue;
				}

				const char* begin = m_current;
				while ( std::isdigit( *m_current ) != 0 )
				{
					++m_current;
				}

				std::string numberStr( begin, m_current - begin );
				val *= atoi( numberStr.c_str() );

				if ( val < 0 )
				{
					switch ( col )
					{
					case 0:
						face.m_vertices.push_back( vSize + val );
						break;
					case 1:
						face.m_texcoords.push_back( vtSize + val );
						break;
					case 2:
						face.m_normals.push_back( vnSize + val );
						break;
					default:
						break;
					}
				}
				else
				{
					switch ( col )
					{
					case 0:
						face.m_vertices.push_back( val - 1 );
						break;
					case 1:
						face.m_texcoords.push_back( val - 1 );
						break;
					case 2:
						face.m_normals.push_back( val - 1 );
						break;
					default:
						break;
					}
				}
			}
		}

		void ReadGroupName( ObjModel& mesh )
		{
			SkipWhiteSpace();
			std::string groupName = ReadWord();
			CreateObject( groupName, mesh );
		}

		void ReadMaterialName( ObjModel& mesh )
		{
			SkipWhiteSpace();
			std::string name = ReadWord();

			auto found = std::find_if( std::begin( m_materialLut ), std::end( m_materialLut ),
				[&name]( const MaterialList& ml )
				{
					auto found = ml.m_materialNames.find( name );
					return found != std::end( ml.m_materialNames );
				} );

			std::string mtlName;
			if ( found == std::end( m_materialLut ) )
			{
				name = DefaultMaterialName;
			}
			else
			{
				mtlName = found->m_mtlFileName;
			}

			if ( m_curMesh == nullptr )
			{
				CreateObject( name, mesh );
			}

			if ( ( m_curMesh->m_faces.empty() == false )
				&& ( m_curMesh->m_materialLibraryName.empty() == false )
				&& ( m_curMesh->m_materialName.empty() == false ) )
			{
				CreateObject( name, mesh );
			}

			m_curMesh->m_materialLibraryName = std::move( mtlName );
			m_curMesh->m_materialName = std::move( name );
		}

		void MakeMtlLookUpTable()
		{
			SkipWhiteSpace();
			std::string fileName = ReadWord();
			std::filesystem::path mtlPath = m_workingPath / std::filesystem::path( fileName );

			if ( std::filesystem::exists( mtlPath ) )
			{
				std::ifstream mtlFile( mtlPath );

				if ( mtlFile.good() )
				{
					std::string contents;
					std::getline( mtlFile, contents, static_cast<char>( EOF ) );
					mtlFile.close();

					ObjMaterialLibrary mtl;
					ObjMtlParser parser;
					if ( parser.Parse( contents.data(), contents.size(), mtl ) )
					{
						m_materialLut.emplace_back();
						MaterialList& ml = m_materialLut.back();

						ml.m_mtlFileName = mtlPath.stem().generic_string();

						for ( const auto& material : mtl.m_materials )
						{
							ml.m_materialNames.emplace( material.first );
						}
					}
				}
			}
		}

		bool ParseObj( ObjModel& mesh )
		{
			while ( CanRead() )
			{
				Token token = ReadToken();

				switch ( token.m_type )
				{
				case TokenType::Error:
					if ( CanRead() )
					{
						return false;
					}
					break;
				case TokenType::Comment:
					SkipLine();
					break;
				case TokenType::Group:
					ReadGroupName( mesh );
					break;
				case TokenType::Object:
					// Handle object as group
					ReadGroupName( mesh );
					break;
				case TokenType::SmoothingGroup:
					SkipLine();
					break;
				case TokenType::Vertex:
					ReadVec3( mesh.m_vertices );
					break;
				case TokenType::TextureCoord:
					ReadTexcoord( mesh.m_texcoord );
					break;
				case TokenType::Normal:
					ReadVec3( mesh.m_normal );
					break;
				case TokenType::Face:
					ReadFace( mesh );
					break;
				case TokenType::IncludeMaterial:
					MakeMtlLookUpTable();
					break;
				case TokenType::UseMaterial:
					ReadMaterialName( mesh );
					break;
				}
			}

			return true;
		}

		Object* m_curObject = nullptr;
		ObjMesh* m_curMesh = nullptr;

		struct MaterialList
		{
			std::string m_mtlFileName;
			std::set<std::string> m_materialNames;
		};

		std::vector<MaterialList> m_materialLut;
		std::filesystem::path m_workingPath;
	};
}