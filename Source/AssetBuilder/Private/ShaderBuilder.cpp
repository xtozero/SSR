#include "ShaderBuilder.h"

#include "../D3D11/D3D11Shaders.h"
#include "AssetBuilderConfig.h"
#include "LibraryTool/InterfaceFactories.h"
#include "ShaderFileMerger.h"
#include "ShaderTool.h"
#include "UberShader.h"

#include <algorithm>
#include <d3d12.h>
#include <format>
#include <fstream>

namespace fs = std::filesystem;

using ::Microsoft::WRL::ComPtr;
using ::rendercore::IShaderDefineVisitor;
using ::rendercore::IShaderPermutation;

namespace
{
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

			std::vector<BinaryChunk> errorMsgs;
			bool compileSuccess = CompileShaderCombination( shaderFile, shaderType, entryPoint, *permutation.Get(), errorMsgs );
			if ( compileSuccess == false )
			{
				std::cout << "\nAn error occurred while compiling " << shaderDesc.m_assetName << "\n";
				for ( auto& errorMsg : errorMsgs )
				{
					std::cout << errorMsg.Data() << "\n";
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
	if ( m_shaderCompiler.Initialize( D3D_SHADER_MODEL_6_8 ) == false )
	{
		return false;
	}

	return true;
}

bool ShaderBuilder::CompileShaderCombination( const std::string& shaderFile, agl::ShaderType shaderType, const char* entryPoint, IShaderPermutation& permutation, std::vector<BinaryChunk>& outErrorMsgs ) const
{
	bool result = true;

	BinaryChunk shaderSource( static_cast<uint32>( shaderFile.size() ) );
	std::memcpy( shaderSource.Data(), shaderFile.data(), shaderSource.Size() );

	for ( int i = 0; i < permutation.GetPermutationCount(); ++i )
	{
		permutation.SetPermutationId( i );

		class PermutationVisitor : public IShaderDefineVisitor
		{
		public:
			virtual void Visit( const char* name, [[maybe_unused]] const wchar_t* nameW, int32 value ) override
			{
				m_defineStorage.emplace_back( name );
				m_defines.push_back( m_defineStorage.back().data() );

				m_defineStorage.emplace_back( std::format( "{}", value ) );
				m_defines.push_back( m_defineStorage.back().data() );
			}

			PermutationVisitor( const IShaderPermutation& permutation )
			{
				m_defineStorage.reserve( permutation.GetDimensionCount() * 2 );
				m_defines.reserve( permutation.GetDimensionCount() * 2 );
			}

			std::vector<std::string> m_defineStorage;
			std::vector<const char*> m_defines;
		} visitor( permutation );

		permutation.ForEachShaderDefine( visitor );

		agl::ShaderCompileResult compileResult;

		if ( agl::ShaderCompiler::CanCompileToDXBC( shaderType ) )
		{
			compileResult = CompileD3D11Shader( shaderSource, visitor.m_defines, shaderType, entryPoint );
		}

		if ( compileResult.Succeeded() == false )
		{
			compileResult = CompileD3D12Shader( shaderSource, visitor.m_defines, shaderType, entryPoint );
		}

		if ( compileResult.Succeeded() == false )
		{
			outErrorMsgs.emplace_back( compileResult.GetErrorMessage() );
			result = false;
		}
	}

	return result;
}

agl::ShaderCompileResult ShaderBuilder::CompileD3D11Shader( const BinaryChunk& source, const std::vector<const char*>& defines, agl::ShaderType shaderType, const char* entryPoint ) const
{
	return m_shaderCompiler.CompileToDXBC( source, defines, shaderType, entryPoint );
}

agl::ShaderCompileResult ShaderBuilder::CompileD3D12Shader( const BinaryChunk& source, const std::vector<const char*>& defines, agl::ShaderType shaderType, const char* entryPoint ) const
{
	return m_shaderCompiler.CompileToDXIL( source, defines, shaderType, entryPoint );
}
