#include "ShaderManufacturer.h"

#include "../D3D11/D3D11Shaders.h"
#include "ManufactureConfig.h"
#include "ShaderFileMerger.h"
#include "ShaderTool.h"
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

using ::Microsoft::WRL::ComPtr;

namespace
{
	bool ValidateShaderAsset( const AsyncLoadableAsset* asset, const Archive& ar )
	{
		Archive rAr( ar.Data(), ar.Size() );
		uint32 assetId = 0;
		rAr << assetId;

		if ( assetId == rendercore::ComputeShader::Id )
		{
			rendercore::ComputeShader cs;
			cs.Serialize( rAr );

			return ( cs == *reinterpret_cast<const rendercore::ComputeShader*>( asset ) );
		}
		else if ( assetId == rendercore::GeometryShader::Id )
		{
			rendercore::GeometryShader gs;
			gs.Serialize( rAr );

			return ( gs == *reinterpret_cast<const rendercore::GeometryShader*>( asset ) );
		}
		else if ( assetId == rendercore::PixelShader::Id )
		{
			rendercore::PixelShader ps;
			ps.Serialize( rAr );

			return ( ps == *reinterpret_cast<const rendercore::PixelShader*>( asset ) );
		}
		else if ( assetId == rendercore::VertexShader::Id )
		{
			rendercore::VertexShader vs;
			vs.Serialize( rAr );

			return ( vs == *reinterpret_cast<const rendercore::VertexShader*>( asset ) );
		}
		else if ( assetId == rendercore::UberShader::Id )
		{
			rendercore::UberShader uberShader;
			uberShader.Serialize( rAr );

			return ( uberShader == *reinterpret_cast<const rendercore::UberShader*>( asset ) );
		}

		return false;
	}

	const char* GetShaderTargetProfile( agl::ShaderType shaderType )
	{
		switch ( shaderType )
		{
		case agl::ShaderType::None:
			break;
		case agl::ShaderType::VS:
			return "vs_5_0";
			break;
		case agl::ShaderType::HS:
			break;
		case agl::ShaderType::DS:
			break;
		case agl::ShaderType::GS:
			return "gs_5_0";
			break;
		case agl::ShaderType::PS:
			return "ps_5_0";
			break;
		case agl::ShaderType::CS:
			return "cs_5_0";
			break;
		case agl::ShaderType::MS:
			return "ms_6_5";
			break;
		case agl::ShaderType::AS:
			return "as_6_5";
			break;
		case agl::ShaderType::Count:
			break;
		default:
			break;
		}

		assert( false && "Invalid shader type" );
		return "";
	}

	agl::ShaderType GetShaderType( const fs::path& fileName )
	{
		std::string name = fileName.filename().generic_string();

		std::transform( std::begin( name ), std::end( name ), std::begin( name ),
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
		else if ( name.starts_with( "ms" ) )
		{
			return agl::ShaderType::MS;
		}
		else if ( name.starts_with( "as" ) )
		{
			return agl::ShaderType::AS;
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
			TokenType m_type = TokenType::Error;
			std::string_view m_str;
			int32 m_integer = 0;
		};

		std::optional<Token> ReadInteger();
		Token ReadToken();
		bool IsReservedKeyword( std::string_view& keyword ) const;
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
			Token token = {
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
		static constexpr const char* ReservedKeywords[] =
		{
			"__cplusplus"
		};

		for ( const char* reservedKeyword : ReservedKeywords )
		{
			if ( keyword == reservedKeyword )
			{
				return true;
			}
		}

		return false;
	}

	ShaderCompileResult CompileD3D11Shader( const std::string& shaderFile, const char* featureLevel, const rendercore::StaticShaderSwitches& switches )
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

		ComPtr<ID3DBlob> byteCode = nullptr;
		ComPtr<ID3DBlob> errorMsg = nullptr;
		HRESULT hr = D3DCompile( shaderFile.c_str(),
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

		if ( SUCCEEDED( hr ) )
		{
			errorMsg = nullptr;
		}

		return ShaderCompileResult( errorMsg, errorMsg ? static_cast<const char*>( errorMsg->GetBufferPointer() ) : nullptr );
	}

	bool HasExplicitSpace( const char* s )
	{
		if ( _strnicmp( s, "register", std::strlen( "register" ) ) != 0 )
		{
			return false;
		}

		// find first (
		const char* openBracket = std::strstr( s, "(" );

		// find first )
		const char* closeBracket = std::strstr( s, ")" );

		std::string_view args( openBracket, closeBracket + 1 );

		return args.find( "space" ) != std::string::npos;
	}

	void ModifyShaderFileForD2D12( std::string& shaderFile, agl::ShaderType shaderType )
	{
		// Replace register( -> Register(
		char* pos = std::strstr( shaderFile.data(), "register(" );
		while ( pos != nullptr )
		{
			if ( HasExplicitSpace( pos ) == false )
			{
				*pos = 'R';
			}
			pos = std::strstr( pos + 1, "register(" );
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

bool ShaderCompileResult::Succeeded() const
{
	return m_errorMsgBlob.Get() == nullptr;
}

const char* ShaderCompileResult::GetErrorMessage() const
{
	return m_errorMsg;
}

ShaderCompileResult::ShaderCompileResult( const Microsoft::WRL::ComPtr<IUnknown>& errorMsgBlob, const char* errorMsg )
	: m_errorMsgBlob( errorMsgBlob )
	, m_errorMsg( errorMsg )
{
}

bool ShaderManufacturer::IsSuitable( const std::filesystem::path& srcPath ) const
{
	fs::path extension = ToLower( srcPath.extension().generic_string() );
	return extension == fs::path(".fx");
}

std::optional<Products> ShaderManufacturer::Manufacture( const PathEnvironment& env, const std::filesystem::path& path ) const
{
	const json::Value* pIncludePath = env.m_orignal->Find( "include_path" );
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

		ShaderTool shaderTool;
		shaderFile = std::move( shaderTool.Process( shaderFile ) );

		agl::ShaderType shaderType = GetShaderType( path.filename() );
		ModifyShaderFileForD2D12( shaderFile, shaderType );

		StaticSwitchParser parser( shaderFile.data(), shaderFile.length() );
		rendercore::StaticShaderSwitches shaderSwitches;
		shaderSwitches.SetConfigs( parser.Parse() );

		uint32 bias = 1;
		for ( auto& [name, shaderSwitch] : shaderSwitches.Configs() )
		{
			shaderSwitch.m_bias = bias;
			bias *= shaderSwitch.NumShaderValues();
		}

		std::vector<ShaderCompileResult> errorMsgs;
		std::set<uint32> compiledShaderIDs = CompileShaderCombination( shaderFile, shaderType, shaderSwitches, errorMsgs );
		if ( compiledShaderIDs.empty() )
		{
			std::cout << "\nAn error occurred while compiling " << path.filename().generic_string() << "\n";
			for ( auto& errorMsg : errorMsgs )
			{
				std::cout << errorMsg.GetErrorMessage() << "\n";
			}
			std::cout << "\n";
			return {};
		}

		rendercore::UberShader shader;
		shader.SetName( path.filename().generic_string() );
		shader.SetShaderType( shaderType );

		Name shaderTargetProfile( GetShaderTargetProfile( shaderType ) );
		shader.SetProfile( shaderTargetProfile );

		shader.SetShaderCode( shaderFile );
		shader.SetSwitches( shaderSwitches );

		for ( uint32 id : compiledShaderIDs )
		{
			shader.AddValidVariation( id );
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

bool ShaderManufacturer::Initialize()
{
	HRESULT hr = DxcCreateInstance( CLSID_DxcCompiler, IID_PPV_ARGS( m_compiler.GetAddressOf() ) );
	return SUCCEEDED( hr );
}

std::set<uint32> ShaderManufacturer::CompileShaderCombination( const std::string& shaderFile, agl::ShaderType shaderType, const rendercore::StaticShaderSwitches& switches, std::vector<ShaderCompileResult>& outErrorMsgs ) const
{
	std::set<uint32> compiledShaderIDs;

	rendercore::StaticShaderSwitches copySwitches = switches;
	CompileShaderCombination( shaderFile, shaderType, copySwitches, 0, compiledShaderIDs, outErrorMsgs );

	return compiledShaderIDs;
}

void ShaderManufacturer::CompileShaderCombination( const std::string& shaderFile, agl::ShaderType shaderType, rendercore::StaticShaderSwitches& switches, int32 depth, std::set<uint32>& outCompiledShaderIDs, std::vector<ShaderCompileResult>& outErrorMsgs ) const
{
	if ( switches.Configs().size() == depth )
	{
		ShaderCompileResult compileResult;

		const char* targetProfile = GetShaderTargetProfile( shaderType );
		if ( ( shaderType == agl::ShaderType::MS ) || ( shaderType == agl::ShaderType::AS ) )
		{
			compileResult = CompileD3D12Shader( shaderFile, targetProfile, switches );
		}
		else
		{
			compileResult = CompileD3D11Shader( shaderFile, targetProfile, switches );
		}
		
		if ( compileResult.Succeeded() )
		{
			auto result = outCompiledShaderIDs.emplace( switches.GetId() );
			assert( result.second );
		}
		else
		{
			outErrorMsgs.emplace_back( compileResult );
		}

		return;
	}

	auto iter = std::begin( switches.Configs() );
	std::advance( iter, depth );

	const rendercore::StaticShaderSwitch& curSwitch = iter->second;

	switches.Off( iter->first );
	CompileShaderCombination( shaderFile, shaderType, switches, depth + 1, outCompiledShaderIDs, outErrorMsgs );

	for ( int32 i = curSwitch.m_min; i <= curSwitch.m_max; ++i )
	{
		switches.On( iter->first, i );
		CompileShaderCombination( shaderFile, shaderType, switches, depth + 1, outCompiledShaderIDs, outErrorMsgs );
	}
}

ShaderCompileResult ShaderManufacturer::CompileD3D12Shader( const std::string& shaderFile, const char* featureLevel, const rendercore::StaticShaderSwitches& switches ) const
{
	DxcBuffer buffer = {
		.Ptr = shaderFile.data(),
		.Size = shaderFile.size(),
		.Encoding = DXC_CP_ACP
	};

	std::vector<const wchar_t*> args;

	// entry point
	args.push_back( L"-E" );
	args.push_back( L"main" );

	// target profile
	args.push_back( L"-T" );

	wchar_t wFeatureLevel[8] = {};
	{
		ToWideChar( wFeatureLevel, std::extent_v<decltype( wFeatureLevel )>, featureLevel );
	}

	args.push_back( wFeatureLevel );

	// defines
	const auto& configs = switches.Configs();

	constexpr int32 MaxDefineLen = 256;
	std::vector<std::array<wchar_t, MaxDefineLen>> defineStorage;
	defineStorage.reserve( configs.size() );

	for ( const auto& [name, shaderSwitch] : configs )
	{
		if ( shaderSwitch.m_on == false )
		{
			continue;
		}

		std::array<char, MaxDefineLen> define;
		SPrintf( define.data(), MaxDefineLen, "%s=%s", name.Str().data(), std::to_string( shaderSwitch.m_current ).c_str() );

		defineStorage.emplace_back();
		std::array<wchar_t, MaxDefineLen>& wDefine = defineStorage.back();

		ToWideChar( wDefine.data(), MaxDefineLen, define.data() );

		args.push_back( L"-D" );
		args.push_back( wDefine.data() );
	}

	ComPtr<IDxcResult> results;
	m_compiler->Compile( &buffer
		, args.data()
		, static_cast<uint32>( args.size() )
		, nullptr
		, IID_PPV_ARGS( results.GetAddressOf() ) );

	HRESULT hr = S_OK;
	results->GetStatus( &hr );

	ComPtr<IDxcBlob> byteCode = nullptr;
	ComPtr<IDxcBlobEncoding> errorMsg = nullptr;
	if ( SUCCEEDED( hr ) )
	{
		ComPtr<IDxcBlobUtf16> shaderName = nullptr;
		results->GetOutput( DXC_OUT_OBJECT, IID_PPV_ARGS( byteCode.GetAddressOf() ), shaderName.GetAddressOf() );
	}
	else
	{
		results->GetErrorBuffer( errorMsg.GetAddressOf() );
	}

	return ShaderCompileResult( errorMsg, errorMsg ? static_cast<const char*>( errorMsg->GetBufferPointer() ) : nullptr );
}
