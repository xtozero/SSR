#include "ShaderManufacturer.h"

#include "../D3D11/D3D11Shaders.h"
#include "ManufactureConfig.h"
#include "ShaderFileMerger.h"
#include "StaticShaderSwitch.h"
#include "TextTokenaizer.h"
#include "UberShader.h"

#include <algorithm>
#include <charconv>
#include <compare>
#include <format>
#include <fstream>
#include <map>
#include <set>

namespace fs = std::filesystem;

namespace
{
	bool ValidateShaderAsset( const AsyncLoadableAsset* asset, const Archive& ar )
	{
		Archive rAr( ar.Data(), ar.Size() );
		uint32 assetID = 0;
		rAr << assetID;

		if ( assetID == rendercore::ComputeShader::ID )
		{
			rendercore::ComputeShader cs;
			cs.Serialize( rAr );

			return ( cs == *reinterpret_cast<const rendercore::ComputeShader*>( asset ) );
		}
		else if ( assetID == rendercore::GeometryShader::ID )
		{
			rendercore::GeometryShader gs;
			gs.Serialize( rAr );

			return ( gs == *reinterpret_cast<const rendercore::GeometryShader*>( asset ) );
		}
		else if ( assetID == rendercore::PixelShader::ID )
		{
			rendercore::PixelShader ps;
			ps.Serialize( rAr );

			return ( ps == *reinterpret_cast<const rendercore::PixelShader*>( asset ) );
		}
		else if ( assetID == rendercore::VertexShader::ID )
		{
			rendercore::VertexShader vs;
			vs.Serialize( rAr );

			return ( vs == *reinterpret_cast<const rendercore::VertexShader*>( asset ) );
		}
		else if ( assetID == rendercore::UberShader::ID )
		{
			rendercore::UberShader uberShader;
			uberShader.Serialize( rAr );

			return ( uberShader == *reinterpret_cast<const rendercore::UberShader*>( asset ) );
		}

		return false;
	}

	const char* GetShaderFeatureLevel( const fs::path& fileName )
	{
		std::string name = fileName.filename().generic_string();
		
		std::transform( name.begin(), name.end(), name.begin(),
			[]( unsigned char c )
			{ 
				return static_cast<char>( std::tolower( c ) ); 
			} );

		if ( name.starts_with( "vs" ) )
		{
			return "vs_5_0";
		}
		else if ( name.starts_with( "gs" ) )
		{
			return "gs_5_0";
		}
		else if ( name.starts_with( "ps" ) )
		{
			return "ps_5_0";
		}
		else if ( name.starts_with( "cs" ) )
		{
			return "cs_5_0";
		}

		assert( false && "Invalid shader file name" );
		return "";
	}

	agl::ShaderType GetShaderType( const fs::path& fileName )
	{
		std::string name = fileName.filename().generic_string();

		std::transform( name.begin(), name.end(), name.begin(),
			[]( unsigned char c )
			{
				return static_cast<char>( std::tolower( c ) );
			} );

		if ( name.starts_with( "vs" ) )
		{
			return agl::ShaderType::VS;
		}
		else if ( name.starts_with( "gs" ) )
		{
			return agl::ShaderType::GS;
		}
		else if ( name.starts_with( "ps" ) )
		{
			return agl::ShaderType::PS;
		}
		else if ( name.starts_with( "cs" ) )
		{
			return agl::ShaderType::CS;
		}

		assert( false && "Invalid shader file name" );
		return agl::ShaderType::None;
	}

	class StaticSwitchParser : TextTokenaizer
	{
	public:
		std::map<Name, rendercore::StaticShaderSwitch> Parse();

		StaticSwitchParser( const char* contents, size_t size );

	private:
		enum class TokenType : uint8
		{
			Error = 0,
			If,
			Elif,
			IfDef,
			Define,
			Comparision,
			String,
			Integer,
		};

		struct Token
		{
			TokenType m_type;
			std::string_view m_str;
			int32 m_integer;
		};

		std::optional<Token> ReadInteger();
		Token ReadToken();
		bool IsReservedKeyword( std::string_view& keyword ) const;

		static constexpr const char* ReservedKeywords[] =
		{
			"__cplusplus",
		};
	};

	std::map<Name, rendercore::StaticShaderSwitch> StaticSwitchParser::Parse()
	{
		std::map<Name, rendercore::StaticShaderSwitch> switches;
		std::set<Name> defines;

		while ( CanRead() )
		{
			Token token = ReadToken();

			if ( ( token.m_type == TokenType::If )
				|| ( token.m_type == TokenType::Elif )
				|| ( token.m_type == TokenType::IfDef ) )
			{
				token = ReadToken();
				if ( token.m_type == TokenType::String )
				{
					if ( IsReservedKeyword( token.m_str ) )
					{
						continue;
					}

					auto result = switches.emplace( token.m_str, rendercore::StaticShaderSwitch() );
					if ( result.second )
					{
						auto& shaderSwitch = result.first->second;

						shaderSwitch.m_on = false;
						shaderSwitch.m_current = 0;
						shaderSwitch.m_min = 0;
						shaderSwitch.m_max = 0;
					}
					
					const char* curPos = Tell();
					token = ReadToken();

					if ( token.m_type != TokenType::Comparision )
					{
						Seek( curPos );
						continue;
					}

					token = ReadToken();

					if ( token.m_type == TokenType::Integer )
					{
						auto& shaderSwitch = result.first->second;

						shaderSwitch.m_min = (std::min)( shaderSwitch.m_min, token.m_integer );
						shaderSwitch.m_max = (std::max)( shaderSwitch.m_max, token.m_integer );
						shaderSwitch.m_current = std::clamp( shaderSwitch.m_current, shaderSwitch.m_min, shaderSwitch.m_max );
					}
				}
				else
				{
					assert( false && "After #if, #elif, #ifdef must be string" );
				}
			}
			else if ( token.m_type == TokenType::Define )
			{
				token = ReadToken();

				if ( token.m_type == TokenType::String )
				{
					defines.emplace( std::string( token.m_str ) );
				}
				else
				{
					assert( false && "After #define must be string" );
				}
			}
		}

		for ( const auto& define : defines )
		{
			switches.erase( define );
		}

		return switches;
	}

	StaticSwitchParser::StaticSwitchParser( const char* contents, size_t length )
	{
		TextTokenaizer::Parse( contents, length );
	}

	std::optional<StaticSwitchParser::Token> StaticSwitchParser::ReadInteger()
	{
		SkipWhiteSpace();

		std::string_view word = ReadWord();
		
		int32 integer = 0;
		std::from_chars_result result = std::from_chars( word.data(), word.data() + word.size(), integer );

		if ( result.ec == std::errc() )
		{
			Token token =
			{
				.m_type = TokenType::Integer,
				.m_str = word,
				.m_integer = integer

			};

			return token;
		}

		return {};
	}

	StaticSwitchParser::Token StaticSwitchParser::ReadToken()
	{
		SkipWhiteSpace();

		Token token;
		token.m_type = TokenType::Error;

		if ( CanRead() == false )
		{
			return token;
		}

		switch ( PeekNextChar() )
		{
		case '#':
		{
			std::string_view word = ReadWord();
			if ( word.starts_with( "#if" ) )
			{
				token.m_type = TokenType::If;
			}
			else if ( word.starts_with( "#elif" ) )
			{
				token.m_type = TokenType::Elif;
			}
			else if ( word.starts_with( "#ifdef" ) )
			{
				token.m_type = TokenType::IfDef;
			}
			else if ( word.starts_with( "#define" ) )
			{
				token.m_type = TokenType::Define;
			}
			token.m_str = word;
			break;
		}
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
		{
			auto integer = ReadInteger();
			if ( integer.has_value() )
			{
				token = integer.value();
			}
			break;
		}
		case '!':
			[[fallthrough]];
		case '<':
			[[fallthrough]];
		case '>':
			[[fallthrough]];
		case '=':
			token.m_str = ReadWord();
			if ( token.m_str == "<"
				|| token.m_str == "<="
				|| token.m_str == ">"
				|| token.m_str == ">="
				|| token.m_str == "=="
				|| token.m_str == "!=" )
			{
				token.m_type = TokenType::Comparision;
			}
			break;
		default:
			token.m_type = TokenType::String;
			token.m_str = ReadWord();
		}

		return token;
	}

	bool StaticSwitchParser::IsReservedKeyword( std::string_view& keyword ) const
	{
		for ( const char* reservedKeyword : ReservedKeywords )
		{
			if ( keyword == reservedKeyword )
			{
				return true;
			}
		}

		return false;
	}

	Microsoft::WRL::ComPtr<ID3DBlob> CompileD3D11Shader( const std::string& shaderFile, const char* featureLevel, const rendercore::StaticShaderSwitches& switches )
	{
		std::vector<D3D_SHADER_MACRO> macros;
		std::vector<std::string> valueStrs;
		const auto& configs = switches.Configs();

		macros.resize( configs.size() + 1 );
		valueStrs.reserve( configs.size() + 1 );
		D3D_SHADER_MACRO* macro = macros.data();
		for ( const auto& [name, shaderSwitch] : configs )
		{
			if ( shaderSwitch.m_on == false )
			{
				continue;
			}

			macro->Name = name.Str().data();
			valueStrs.emplace_back( std::to_string( shaderSwitch.m_current ) );
			macro->Definition = valueStrs.back().c_str();
			++macro;
		}
		macros.back().Name = nullptr;
		macros.back().Definition = nullptr;

		Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errorMsg = nullptr;
		D3DCompile( shaderFile.c_str(),
			shaderFile.size(),
			nullptr,
			macros.data(),
			nullptr,
			"main",
			featureLevel,
			D3DCOMPILE_ENABLE_STRICTNESS,
			0,
			&byteCode,
			&errorMsg );

		return byteCode;
	}

	void ModifyShaderFileForD2D12( std::string& shaderFile, agl::ShaderType shaderType )
	{
		// Replace register( -> Register(
		char* pos = std::strstr( shaderFile.data(), "register(" );
		while ( pos != nullptr )
		{
			*pos = 'R';
			pos = std::strstr( pos, "register(" );
		}

		/* Sample
		* #if D3D12 == 1
		* #define Register(x) register(x, space{shaderType})
		* #else
		* #define Register(x) register(x)
		* #endif
		*/
		std::string macroStrForD3D12 = std::format( "#if D3D12 == 1\n#define Register(x) register(x, space{})\n#else\n#define Register(x) register(x)\n#endif\n\n", static_cast<int32>( shaderType ) );
		shaderFile = macroStrForD3D12 + shaderFile;
	}
}

bool ShaderManufacturer::IsSuitable( const std::filesystem::path& srcPath ) const
{
	return srcPath.extension() == fs::path(".fx");
}

std::optional<Products> ShaderManufacturer::Manufacture( const PathEnvironment& env, const std::filesystem::path& path ) const
{
	const JSON::Value* pIncludePath = env.m_orignal->Find( "include_path" );
	if ( pIncludePath == nullptr )
	{
		return {};
	}

	fs::path includePath = ManufactureConfig::Instance().WorkingDirectory() / fs::path( pIncludePath->AsString() );

	ShaderFileMerger merger( includePath );
	auto merged = merger.Merge( path );
	if ( merged )
	{
		std::string shaderFile = std::move( merged.value() );

		agl::ShaderType shaderType = GetShaderType( path.filename() );
		ModifyShaderFileForD2D12( shaderFile, shaderType );

		StaticSwitchParser parser( shaderFile.data(), shaderFile.length() );
		rendercore::StaticShaderSwitches shaderSwitches;
		shaderSwitches.m_configs = parser.Parse();

		Name shaderFeatureLevel( GetShaderFeatureLevel( path.filename() ) );

		std::map<uint32, Microsoft::WRL::ComPtr<ID3DBlob>> compiledShader;
		CombinationStaticSwitches( shaderFile, std::string( shaderFeatureLevel.Str() ).c_str(), compiledShader, shaderSwitches);
		assert( compiledShader.size() != 0 );

		rendercore::UberShader shader;
		shader.m_name = path.filename().generic_string();
		shader.m_type = shaderType;
		shader.m_profile = shaderFeatureLevel;

		std::construct_at( &shader.m_shaderCode, static_cast<uint32>( shaderFile.length() ) );
		std::memcpy( shader.m_shaderCode.Data(), shaderFile.data(), shaderFile.length() );

		shader.m_switches = shaderSwitches;

		for ( auto& [id, byteCode] : compiledShader )
		{
			shader.m_validVariation.emplace( id );
		}

		shader.SetLastWriteTime( fs::last_write_time( path ) );

		Archive ar;
		shader.Serialize( ar );

#ifdef ASSET_VALIDATE
		if ( ValidateShaderAsset( &shader, ar ) == false )
		{
			DebugBreak();
		}
#endif

		Products products;
		products.emplace_back( path.filename(), std::move( ar ) );
		return products;
	}

	return {};
}

void ShaderManufacturer::CombinationStaticSwitches( const std::string& shaderFile, const char* featureLevel, std::map<uint32, Microsoft::WRL::ComPtr<ID3DBlob>>& outCompiledShaders, const rendercore::StaticShaderSwitches& switches ) const
{
	rendercore::StaticShaderSwitches copySwitches = switches;
	CombinationStaticSwitchesRecursive( shaderFile, featureLevel, outCompiledShaders, copySwitches, 0 );
}

void ShaderManufacturer::CombinationStaticSwitchesRecursive( const std::string& shaderFile, const char* featureLevel, std::map<uint32, Microsoft::WRL::ComPtr<ID3DBlob>>& outCompiledShaders, rendercore::StaticShaderSwitches& switches, int32 depth ) const
{
	if ( switches.m_configs.size() == depth )
	{
		Microsoft::WRL::ComPtr<ID3DBlob> compiled = CompileD3D11Shader( shaderFile, featureLevel, switches );
		if ( compiled )
		{
			auto result = outCompiledShaders.emplace( switches.GetID(), compiled );
			assert( result.second );
		}

		return;
	}

	auto iter = switches.m_configs.begin();
	std::advance( iter, depth );

	const rendercore::StaticShaderSwitch& curSwitch = iter->second;

	switches.Off( iter->first );
	CombinationStaticSwitchesRecursive( shaderFile, featureLevel, outCompiledShaders, switches, depth + 1 );

	for ( int32 i = curSwitch.m_min; i <= curSwitch.m_max; ++i )
	{
		switches.On( iter->first, i );
		CombinationStaticSwitchesRecursive( shaderFile, featureLevel, outCompiledShaders, switches, depth + 1 );
	}
}
