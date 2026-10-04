#include "ShaderCompiler/ShaderCompiler.h"

#include "ShaderResource.h"

#include <d3dcompiler.h>

using ::Microsoft::WRL::ComPtr;

namespace agl
{
	bool ShaderCompileResult::Succeeded() const
	{
		return m_byteCode.Size() > 0 && m_errorMessage.Size() == 0;
	}

	const BinaryChunk& ShaderCompileResult::GetByteCode() const
	{
		return m_byteCode;
	}

	const BinaryChunk& ShaderCompileResult::GetErrorMessage() const
	{
		return m_errorMessage;
	}

	bool ShaderCompiler::Initialize( D3D_SHADER_MODEL shaderModel )
	{
		assert( shaderModel != D3D_SHADER_MODEL_NONE && "Invalid shader model" );

		m_shaderModel = shaderModel;

		HRESULT hr = DxcCreateInstance( CLSID_DxcCompiler, IID_PPV_ARGS( m_compiler.GetAddressOf() ) );
		if ( FAILED( hr ) )
		{
			return false;
		}

		return true;
	}

    ShaderCompileResult ShaderCompiler::CompileToDXIL( const BinaryChunk& source, const std::vector<const char*>& defines, ShaderType type, const char* entryPoint ) const
    {
        DxcBuffer buffer = {
			.Ptr = source.Data(),
			.Size = source.Size(),
			.Encoding = DXC_CP_ACP
		};

		std::vector<const wchar_t*> args;
		args.reserve( 64 );

		// entry point
		if ( IsRaytracingShader( type ) )
		{
			args.push_back( L"-auto-binding-space 0" );
			args.push_back( L"-exports" );
		}
		else
		{
			args.push_back( L"-E" );
		}

		wchar_t wEntryPoint[64] = {};
		{
			ToWideChar( wEntryPoint, std::extent_v<decltype( wEntryPoint )>, entryPoint );
		}
		args.push_back( wEntryPoint );

		// for vulkan
		if ( GetInterface<IAgl>()->GetType() == AglType::Vulkan )
		{
			args.push_back( L"-spirv" );
		}

		// target profile
		args.push_back( L"-T" );
		args.push_back( GetShaderProfileForDXIL( type ) );

#if _DEBUG
		args.push_back( L"-Zs" );
#endif

		// defines
		args.push_back( L"-D" );
		args.push_back( L"D3D12=1");

		constexpr int32 MaxDefineLen = 256;
		std::vector<std::array<wchar_t, MaxDefineLen>> defineStorage;
		defineStorage.reserve( defines.size() / 2 );

		for ( uint32 i = 0; i < defines.size(); i += 2 )
		{
			if ( defines[i] == nullptr || defines[i + 1] == nullptr )
			{
				continue;
			}

			std::array<char, MaxDefineLen> define;

			SPrintf( define.data(), MaxDefineLen, "%s=%s", defines[i], defines[i + 1] );

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

		ShaderCompileResult result;
		if ( SUCCEEDED( hr ) )
		{
			ComPtr<IDxcBlob> compiledBinary = nullptr;
			ComPtr<IDxcBlobUtf16> shaderName = nullptr;
			results->GetOutput( DXC_OUT_OBJECT, IID_PPV_ARGS( compiledBinary.GetAddressOf() ), shaderName.GetAddressOf() );

			assert( compiledBinary.Get() != nullptr );

			result.m_byteCode.Resize( static_cast<uint32>( compiledBinary->GetBufferSize() ) );
			std::memcpy( result.m_byteCode.Data(), compiledBinary->GetBufferPointer(), compiledBinary->GetBufferSize() );
		}
		else
		{
			ComPtr<IDxcBlobEncoding> errorMsg = nullptr;
			results->GetErrorBuffer( errorMsg.GetAddressOf() );

			result.m_errorMessage.Resize( errorMsg->GetBufferSize() );
			std::memcpy( result.m_errorMessage.Data(), errorMsg->GetBufferPointer(), errorMsg->GetBufferSize() );
		}

		return result;
    }

    ShaderCompileResult ShaderCompiler::CompileToDXBC( const BinaryChunk& source, const std::vector<const char*>& defines, ShaderType type, const char* entryPoint ) const
    {
		ComPtr<ID3DBlob> byteCode = nullptr;
		ComPtr<ID3DBlob> errorMsg = nullptr;

		std::vector<D3D_SHADER_MACRO> macros;

		macros.resize( ( defines.size() >> 1 ) + 1 );
		D3D_SHADER_MACRO* macro = macros.data();
		for ( uint32 i = 0; i < defines.size(); )
		{
			macro->Name = defines[i++];
			macro->Definition = defines[i++];
			++macro;
		}
		macros.back().Name = nullptr;
		macros.back().Definition = nullptr;

		HRESULT hr = D3DCompile( source.Data(),
			source.Size(),
			nullptr,
			macros.data(),
			nullptr,
			entryPoint,
			GetShaderProfileForDXBC( type ),
			D3DCOMPILE_ENABLE_STRICTNESS,
			0,
			&byteCode,
			&errorMsg );

		ShaderCompileResult result;
		if ( SUCCEEDED( hr ) )
		{
			result.m_byteCode.Resize( static_cast<uint32>( byteCode->GetBufferSize() ) );
			std::memcpy( result.m_byteCode.Data(), byteCode->GetBufferPointer(), byteCode->GetBufferSize() );
		}
		else
		{
			result.m_errorMessage.Resize( errorMsg->GetBufferSize() );
			std::memcpy( result.m_errorMessage.Data(), errorMsg->GetBufferPointer(), errorMsg->GetBufferSize() );
		}

		return result;
    }

    bool ShaderCompiler::CanCompileToDXBC( ShaderType shaderType )
    {
	    return shaderType == ShaderType::Vertex
		    || shaderType == ShaderType::Hull
		    || shaderType == ShaderType::Domain
		    || shaderType == ShaderType::Geometry
		    || shaderType == ShaderType::Pixel
		    || shaderType == ShaderType::Compute;
    }

    const wchar_t* ShaderCompiler::GetShaderProfileForDXIL( ShaderType type ) const
	{
		if ( type == ShaderType::Vertex )
		{
			switch ( m_shaderModel )
			{
			case D3D_SHADER_MODEL_6_5:
				return L"vs_6_5";
			case D3D_SHADER_MODEL_6_6:
				return L"vs_6_6";
			case D3D_SHADER_MODEL_6_7:
				return L"vs_6_7";
			case D3D_SHADER_MODEL_6_8:
				return L"vs_6_8";
			case D3D_SHADER_MODEL_6_9:
				return L"vs_6_9";
			}
		}
		else if ( type == ShaderType::Geometry )
		{
			switch ( m_shaderModel )
			{
			case D3D_SHADER_MODEL_6_5:
				return L"gs_6_5";
			case D3D_SHADER_MODEL_6_6:
				return L"gs_6_6";
			case D3D_SHADER_MODEL_6_7:
				return L"gs_6_7";
			case D3D_SHADER_MODEL_6_8:
				return L"gs_6_8";
			case D3D_SHADER_MODEL_6_9:
				return L"gs_6_9";
			}
		}
		else if ( type == ShaderType::Pixel )
		{
			switch ( m_shaderModel )
			{
			case D3D_SHADER_MODEL_6_5:
				return L"ps_6_5";
			case D3D_SHADER_MODEL_6_6:
				return L"ps_6_6";
			case D3D_SHADER_MODEL_6_7:
				return L"ps_6_7";
			case D3D_SHADER_MODEL_6_8:
				return L"ps_6_8";
			case D3D_SHADER_MODEL_6_9:
				return L"ps_6_9";
			}
		}
		else if ( type == ShaderType::Compute )
		{
			switch ( m_shaderModel )
			{
			case D3D_SHADER_MODEL_6_5:
				return L"cs_6_5";
			case D3D_SHADER_MODEL_6_6:
				return L"cs_6_6";
			case D3D_SHADER_MODEL_6_7:
				return L"cs_6_7";
			case D3D_SHADER_MODEL_6_8:
				return L"cs_6_8";
			case D3D_SHADER_MODEL_6_9:
				return L"cs_6_9";
			}
		}
		else if ( type == ShaderType::Amplification )
		{
			switch ( m_shaderModel )
			{
			case D3D_SHADER_MODEL_6_5:
				return L"as_6_5";
			case D3D_SHADER_MODEL_6_6:
				return L"as_6_6";
			case D3D_SHADER_MODEL_6_7:
				return L"as_6_7";
			case D3D_SHADER_MODEL_6_8:
				return L"as_6_8";
			case D3D_SHADER_MODEL_6_9:
				return L"as_6_9";
			}
		}
		else if ( type == ShaderType::Mesh )
		{
			switch ( m_shaderModel )
			{
			case D3D_SHADER_MODEL_6_5:
				return L"ms_6_5";
			case D3D_SHADER_MODEL_6_6:
				return L"ms_6_6";
			case D3D_SHADER_MODEL_6_7:
				return L"ms_6_7";
			case D3D_SHADER_MODEL_6_8:
				return L"ms_6_8";
			case D3D_SHADER_MODEL_6_9:
				return L"ms_6_9";
			}
		}
		else if ( IsRaytracingShader( type ) )
		{
			switch ( m_shaderModel )
			{
			case D3D_SHADER_MODEL_6_5:
				return L"lib_6_5";
			case D3D_SHADER_MODEL_6_6:
				return L"lib_6_6";
			case D3D_SHADER_MODEL_6_7:
				return L"lib_6_7";
			case D3D_SHADER_MODEL_6_8:
				return L"lib_6_8";
			case D3D_SHADER_MODEL_6_9:
				return L"lib_6_9";
			}
		}

		assert( false && "Invalid shader type" );
		return L"";
	}

    const char* ShaderCompiler::GetShaderProfileForDXBC( ShaderType type ) const
    {
		switch ( type )
		{
		case ShaderType::Vertex:
			return "vs_5_0";
		case ShaderType::Geometry:
			return "gs_5_0";
		case ShaderType::Pixel:
			return "ps_5_0";
		case ShaderType::Compute:
			return "cs_5_0";
		}

		assert( false && "Invalid shader type" );
		return "";
    }
}
