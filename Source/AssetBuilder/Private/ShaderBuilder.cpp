#include "ShaderBuilder.h"

#include "../D3D11/D3D11Shaders.h"
#include "AssetBuilderConfig.h"
#include "LibraryTool/InterfaceFactories.h"
#include "ShaderFileMerger.h"
#include "ShaderTool.h"
#include "UberShader.h"

#include <algorithm>
#include <d3dcompiler.h>
#include <format>
#include <fstream>

namespace fs = std::filesystem;

using ::Microsoft::WRL::ComPtr;
using ::rendercore::IShaderDefineVisitor;
using ::rendercore::IShaderPermutation;

namespace
{
	std::string GetD3DShaderTargetProfile( agl::ShaderType shaderType, agl::AglType aglType )
	{
		if ( shaderType == agl::ShaderType::None || shaderType == agl::ShaderType::Count )
		{
			return "";
		}

		if ( aglType == agl::AglType::D3D11 )
		{
			static const char* d3d11Profiles[agl::NumShaderTypes<uint32>] = {
				"vs_5_0",
				"hs_5_0",
				"ds_5_0",
				"gs_5_0",
				"ps_5_0",
				"cs_5_0",
				"",
				"",
				"",
				"",
				"",
				"",
				"",
				"",
			};

			return d3d11Profiles[static_cast<int8>( shaderType )];
		}
		else // agl::AglType::D3D12
		{
			static const char* d3d12Profiles[agl::NumShaderTypes<uint32>] = {
				"vs_6_5",
				"hs_6_5",
				"ds_6_5",
				"gs_6_5",
				"ps_6_5",
				"cs_6_5",
				"ms_6_5",
				"as_6_5",
				"lib_6_8",
				"lib_6_8",
				"lib_6_8",
				"lib_6_8",
				"lib_6_8",
				"lib_6_8",
			};

			return d3d12Profiles[static_cast<int8>( shaderType )];
		}
	}

	ShaderCompileResult CompileD3D11Shader( const std::string& shaderFile, const char* entryPoint, const char* featureLevel, const IShaderPermutation& permutation )
	{
		class PermutationVisitor : public IShaderDefineVisitor
		{
		public:
			virtual void Visit( const char* name, [[maybe_unused]] const wchar_t* nameW, int32 value ) override
			{
				m_valueStrs.emplace_back( std::to_string( value ) );
				m_macros.emplace_back( name, m_valueStrs.back().c_str() );
			}

			PermutationVisitor( const IShaderPermutation& permutation )
			{
				m_valueStrs.reserve( permutation.GetDimensionCount() );
				m_macros.reserve( permutation.GetDimensionCount() );
			}

			std::vector<D3D_SHADER_MACRO> m_macros;
			std::vector<std::string> m_valueStrs;
		} visitor( permutation );

		permutation.ForEachShaderDefine( visitor );
		visitor.m_macros.emplace_back( nullptr, nullptr );

		ComPtr<ID3DBlob> byteCode = nullptr;
		ComPtr<ID3DBlob> errorMsg = nullptr;
		HRESULT hr = D3DCompile( shaderFile.c_str(),
			shaderFile.size(),
			nullptr,
			visitor.m_macros.data(),
			nullptr,
			entryPoint,
			featureLevel,
			D3DCOMPILE_ENABLE_STRICTNESS,
			0,
			&byteCode,
			&errorMsg );

		bool compileSucceeded = SUCCEEDED( hr );
		if ( compileSucceeded )
		{
			errorMsg = nullptr;
		}

		return ShaderCompileResult( compileSucceeded, errorMsg );
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
	return m_succeeded;
}

const char* ShaderCompileResult::GetErrorMessage() const
{
	ComPtr<IDxcBlobEncoding> blobEncoding;
	HRESULT hr = m_errorMsgBlob.As( &blobEncoding );
	if ( SUCCEEDED( hr ) )
	{
		return static_cast<const char*>( blobEncoding->GetBufferPointer() );
	}

	ComPtr<ID3DBlob> blob;
	hr = m_errorMsgBlob.As( &blob );
	if ( SUCCEEDED( hr ) )
	{
		return static_cast<const char*>( blob->GetBufferPointer() );
	}

	return "";
}

ShaderCompileResult::ShaderCompileResult( bool succeeded, const Microsoft::WRL::ComPtr<IUnknown>& errorMsgBlob )
	: m_succeeded( succeeded )
	, m_errorMsgBlob( errorMsgBlob )
{
}

bool ShaderBuilder::IsSuitable( const std::filesystem::path& srcPath ) const
{
	fs::path extension = ToLower( srcPath.extension().generic_string() );
	return extension == fs::path(".fx");
}

std::optional<Products> ShaderBuilder::Build( const PathEnvironment& env, const std::filesystem::path& path ) const
{
	const json::Value* pIncludePath = env.m_orignal->Find( "include_path" );
	if ( pIncludePath == nullptr )
	{
		return {};
	}

	fs::path includePath = AssetBuilderConfig::Instance().WorkingDirectory() / fs::path( pIncludePath->AsString() );

	auto shaderRegistry = GetInterface<rendercore::IShaderRegistry>();
	fs::path relativePath = fs::relative( path );
	const std::vector<rendercore::ShaderDescriptor>* shaderDescs = shaderRegistry->Find( relativePath );

	if ( shaderDescs == nullptr )
	{
		std::cout << std::format( "ShaderDescriptor for {} not found. Please register the shader using REGISTER_SHADER.\n", relativePath.generic_string() );
		return {};
	}

	uint32 shaderDescriptorHandle = rendercore::ShaderDescriptorHandle( relativePath.generic_string().c_str() );

	Products products;
	for ( const auto& shaderDesc : *shaderDescs )
	{
		ShaderFileMerger merger( includePath );
		auto merged = merger.Merge( path );
		if ( merged )
		{
			std::string shaderFile = std::move( merged.value() );

			ShaderTool shaderTool;
			shaderFile = std::move( shaderTool.Process( shaderFile ) );

			agl::ShaderType shaderType = shaderDesc.m_type;
			ModifyShaderFileForD2D12( shaderFile, shaderType );

			const char* entryPoint = shaderDesc.m_entryPoint.data();

			auto permutation = shaderDesc.m_createPermutationFunc();

			std::vector<ShaderCompileResult> errorMsgs;
			bool compileSuccess = CompileShaderCombination( shaderFile, shaderType, entryPoint, *permutation.Get(), errorMsgs );
			if ( compileSuccess == false )
			{
				std::cout << "\nAn error occurred while compiling " << shaderDesc.m_assetName << "\n";
				for ( auto& errorMsg : errorMsgs )
				{
					std::cout << errorMsg.GetErrorMessage() << "\n";
				}
				std::cout << "\n";
				return {};
			}

			auto shader = std::make_unique<rendercore::UberShader>();
			shader->SetName( shaderDesc.m_assetName );
			shader->SetShaderType( shaderType );
			shader->SetEntryPoint( entryPoint );

			shader->SetShaderCode( shaderFile );
			shader->SetShaderDescriptorHandle( shaderDescriptorHandle );

			products.emplace_back( shaderDesc.m_assetName, std::move( shader ) );
		}
	}

	if ( products.empty() == false )
	{
		return products;
	}

	return {};
}

bool ShaderBuilder::Initialize()
{
	HRESULT hr = DxcCreateInstance( CLSID_DxcCompiler, IID_PPV_ARGS( m_compiler.GetAddressOf() ) );
	return SUCCEEDED( hr );
}

bool ShaderBuilder::CompileShaderCombination( const std::string& shaderFile, agl::ShaderType shaderType, const char* entryPoint, IShaderPermutation& permutation, std::vector<ShaderCompileResult>& outErrorMsgs ) const
{
	bool result = true;

	for ( int i = 0; i < permutation.GetPermutationCount(); ++i )
	{
		permutation.SetPermutationId( i );

		ShaderCompileResult compileResult;

		std::string targetProfile = GetD3DShaderTargetProfile( shaderType, agl::AglType::D3D11 );
		if ( targetProfile.empty() == false )
		{
			compileResult = CompileD3D11Shader( shaderFile, entryPoint, targetProfile.c_str(), permutation );
		}

		if ( compileResult.Succeeded() == false )
		{
			targetProfile = GetD3DShaderTargetProfile( shaderType, agl::AglType::D3D12 );
			compileResult = CompileD3D12Shader( shaderFile, entryPoint, targetProfile.c_str(), permutation );
		}

		if ( compileResult.Succeeded() == false )
		{
			outErrorMsgs.emplace_back( compileResult );
			result = false;
		}
	}

	return result;
}

ShaderCompileResult ShaderBuilder::CompileD3D12Shader( const std::string& shaderFile, const char* entryPoint, const char* featureLevel, const IShaderPermutation& permutation ) const
{
	DxcBuffer buffer = {
		.Ptr = shaderFile.data(),
		.Size = shaderFile.size(),
		.Encoding = DXC_CP_ACP
	};

	std::vector<const wchar_t*> args;

	// entry point
	args.push_back( L"-E" );
	wchar_t wEntryPoint[64] = {};
	{
		ToWideChar( wEntryPoint, std::extent_v<decltype( wEntryPoint )>, entryPoint );
	}
	args.push_back( wEntryPoint );

	// target profile
	args.push_back( L"-T" );
	wchar_t wFeatureLevel[8] = {};
	{
		ToWideChar( wFeatureLevel, std::extent_v<decltype( wFeatureLevel )>, featureLevel );
	}
	args.push_back( wFeatureLevel );

	// defines
	args.push_back( L"-D" );
	args.push_back( L"D3D12=1");

	class PermutationVisitor : public IShaderDefineVisitor
	{
	public:
		virtual void Visit( [[maybe_unused]] const char* name, const wchar_t* nameW, int32 value ) override
		{
			m_defineStorage.emplace_back( std::format( L"{}={}", nameW, value ) );

			m_args.push_back( L"-D" );
			m_args.push_back( m_defineStorage.back().data() );
		}

		PermutationVisitor( const IShaderPermutation& permutation )
		{
			m_defineStorage.reserve( permutation.GetDimensionCount() );
			m_args.reserve( permutation.GetDimensionCount() * 2 );
		}

		std::vector<std::wstring> m_defineStorage;
		std::vector<const wchar_t*> m_args;
	} visitor( permutation );

	permutation.ForEachShaderDefine( visitor );
	args.insert( std::end( args ), std::begin( visitor.m_args ), std::end( visitor.m_args ) );

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
	bool compileSucceeded = SUCCEEDED( hr );
	if ( compileSucceeded )
	{
		ComPtr<IDxcBlobUtf16> shaderName = nullptr;
		results->GetOutput( DXC_OUT_OBJECT, IID_PPV_ARGS( byteCode.GetAddressOf() ), shaderName.GetAddressOf() );
	}
	else
	{
		results->GetErrorBuffer( errorMsg.GetAddressOf() );
	}

	return ShaderCompileResult( compileSucceeded, errorMsg );
}
