#include "stdafx.h"

#include "Config/DefaultAglConfig.h"
#include "D3D11Api.h"
#include "D3D12Api.h"
#include "D3D11ResourceManager.h"
#include "D3D12ResourceManager.h"
#include "EnumStringMap.h"
#include "IAgl.h"
#include "InterfaceFactories.h"

using ::agl::AglType;
using ::agl::Blend;
using ::agl::BlendOp;
using ::agl::ColorWriteEnable;
using ::agl::ComparisonFunc;
using ::agl::CullMode;
using ::agl::DepthWriteMode;
using ::agl::FillMode;
using ::agl::ResourceAccessFlag;
using ::agl::ResourceBindType;
using ::agl::ResourceFormat;
using ::agl::ResourceMisc;
using ::agl::StencilOp;
using ::agl::TextureFilter;
using ::agl::TextureAddressMode;

namespace
{
	agl::IAgl* g_abstractGraphicsLibrary = nullptr;
	agl::IResourceManager* g_resourceManager = nullptr;

	void CreateGraphicsApi()
	{
		if ( agl::DefaultAgl::GetType() == AglType::D3D12 )
		{
			g_abstractGraphicsLibrary = agl::CreateD3D12GraphicsApi();
		}
		else
		{
			g_abstractGraphicsLibrary = agl::CreateD3D11GraphicsApi();
		}
	}

	void CreateResourceManager()
	{
		if ( agl::DefaultAgl::GetType() == AglType::D3D12 )
		{
			g_resourceManager = agl::CreateD3D12ResourceManager();
		}
		else
		{
			g_resourceManager = agl::CreateD3D11ResourceManager();
		}
	}

	void DestoryResourceManager()
	{
		delete g_resourceManager;
	}

	void* GetResourceManager()
	{
		return g_resourceManager;
	}

	void DestoryGraphicsApi()
	{
		delete g_abstractGraphicsLibrary;
	}

	void* GetGraphicsApi()
	{
		return g_abstractGraphicsLibrary;
	}

	void RegisterResourceEnumString()
	{
		// Register enum string
		REGISTER_ENUM_STRING( AglType::D3D11 );
		REGISTER_ENUM_STRING( AglType::D3D12 );

		// Blend
		REGISTER_ENUM_STRING( Blend::Zero );
		REGISTER_ENUM_STRING( Blend::One );
		REGISTER_ENUM_STRING( Blend::SrcColor );
		REGISTER_ENUM_STRING( Blend::InvSrcColor );
		REGISTER_ENUM_STRING( Blend::SrcAlpha );
		REGISTER_ENUM_STRING( Blend::InvSrcAlpha );
		REGISTER_ENUM_STRING( Blend::DestAlpha );
		REGISTER_ENUM_STRING( Blend::InvDestAlpha );
		REGISTER_ENUM_STRING( Blend::DestColor );
		REGISTER_ENUM_STRING( Blend::InvDestColor );
		REGISTER_ENUM_STRING( Blend::SrcAlphaSat );
		REGISTER_ENUM_STRING( Blend::BlendFactor );
		REGISTER_ENUM_STRING( Blend::InvBlendFactor );
		REGISTER_ENUM_STRING( Blend::Src1Color );
		REGISTER_ENUM_STRING( Blend::InvSrc1Color );
		REGISTER_ENUM_STRING( Blend::Src1Alpha );
		REGISTER_ENUM_STRING( Blend::InvSrc1Alpha );

		// Blend operation
		REGISTER_ENUM_STRING( BlendOp::Add );
		REGISTER_ENUM_STRING( BlendOp::Subtract );
		REGISTER_ENUM_STRING( BlendOp::ReverseSubtract );
		REGISTER_ENUM_STRING( BlendOp::Min );
		REGISTER_ENUM_STRING( BlendOp::Max );

		// Color write enable
		REGISTER_ENUM_STRING( ColorWriteEnable::Red );
		REGISTER_ENUM_STRING( ColorWriteEnable::Green );
		REGISTER_ENUM_STRING( ColorWriteEnable::Blue );
		REGISTER_ENUM_STRING( ColorWriteEnable::Alpha );
		REGISTER_ENUM_STRING( ColorWriteEnable::All );

		// Comparision func
		REGISTER_ENUM_STRING( ComparisonFunc::Never );
		REGISTER_ENUM_STRING( ComparisonFunc::Less );
		REGISTER_ENUM_STRING( ComparisonFunc::Equal );
		REGISTER_ENUM_STRING( ComparisonFunc::LessEqual );
		REGISTER_ENUM_STRING( ComparisonFunc::Greater );
		REGISTER_ENUM_STRING( ComparisonFunc::NotEqual );
		REGISTER_ENUM_STRING( ComparisonFunc::GreaterEqual );
		REGISTER_ENUM_STRING( ComparisonFunc::Always );

		// Cull mode
		REGISTER_ENUM_STRING( CullMode::None );
		REGISTER_ENUM_STRING( CullMode::Front );
		REGISTER_ENUM_STRING( CullMode::Back );

		// Depth write mode
		REGISTER_ENUM_STRING( DepthWriteMode::Zero );
		REGISTER_ENUM_STRING( DepthWriteMode::All );

		// Fill mode
		REGISTER_ENUM_STRING( FillMode::Wireframe );
		REGISTER_ENUM_STRING( FillMode::Solid );

		// Resource access flag
		REGISTER_ENUM_STRING( ResourceAccessFlag::GpuRead );
		REGISTER_ENUM_STRING( ResourceAccessFlag::GpuWrite );
		REGISTER_ENUM_STRING( ResourceAccessFlag::CpuRead );
		REGISTER_ENUM_STRING( ResourceAccessFlag::CpuWrite );

		// Resource bind type
		REGISTER_ENUM_STRING( ResourceBindType::VertexBuffer );
		REGISTER_ENUM_STRING( ResourceBindType::IndexBuffer );
		REGISTER_ENUM_STRING( ResourceBindType::ConstantBuffer );
		REGISTER_ENUM_STRING( ResourceBindType::ShaderResource );
		REGISTER_ENUM_STRING( ResourceBindType::StreamOutput );
		REGISTER_ENUM_STRING( ResourceBindType::RenderTarget );
		REGISTER_ENUM_STRING( ResourceBindType::DepthStencil );
		REGISTER_ENUM_STRING( ResourceBindType::RandomAccess );

		// Resource format
		REGISTER_ENUM_STRING( ResourceFormat::Unknown );
		REGISTER_ENUM_STRING( ResourceFormat::R32G32B32A32_TYPELESS );
		REGISTER_ENUM_STRING( ResourceFormat::R32G32B32A32_FLOAT );
		REGISTER_ENUM_STRING( ResourceFormat::R32G32B32A32_UINT );
		REGISTER_ENUM_STRING( ResourceFormat::R32G32B32A32_SINT );
		REGISTER_ENUM_STRING( ResourceFormat::R32G32B32_TYPELESS );
		REGISTER_ENUM_STRING( ResourceFormat::R32G32B32_FLOAT );
		REGISTER_ENUM_STRING( ResourceFormat::R32G32B32_UINT );
		REGISTER_ENUM_STRING( ResourceFormat::R32G32B32_SINT );
		REGISTER_ENUM_STRING( ResourceFormat::R16G16B16A16_TYPELESS );
		REGISTER_ENUM_STRING( ResourceFormat::R16G16B16A16_FLOAT );
		REGISTER_ENUM_STRING( ResourceFormat::R16G16B16A16_UNORM );
		REGISTER_ENUM_STRING( ResourceFormat::R16G16B16A16_UINT );
		REGISTER_ENUM_STRING( ResourceFormat::R16G16B16A16_SNORM );
		REGISTER_ENUM_STRING( ResourceFormat::R16G16B16A16_SINT );
		REGISTER_ENUM_STRING( ResourceFormat::R32G32_TYPELESS );
		REGISTER_ENUM_STRING( ResourceFormat::R32G32_FLOAT );
		REGISTER_ENUM_STRING( ResourceFormat::R32G32_UINT );
		REGISTER_ENUM_STRING( ResourceFormat::R32G32_SINT );
		REGISTER_ENUM_STRING( ResourceFormat::R32G8X24_TYPELESS );
		REGISTER_ENUM_STRING( ResourceFormat::D32_FLOAT_S8X24_UINT );
		REGISTER_ENUM_STRING( ResourceFormat::R32_FLOAT_X8X24_TYPELESS );
		REGISTER_ENUM_STRING( ResourceFormat::X32_TYPELESS_G8X24_UINT );
		REGISTER_ENUM_STRING( ResourceFormat::R10G10B10A2_TYPELESS );
		REGISTER_ENUM_STRING( ResourceFormat::R10G10B10A2_UNORM );
		REGISTER_ENUM_STRING( ResourceFormat::R10G10B10A2_UINT );
		REGISTER_ENUM_STRING( ResourceFormat::R11G11B10_FLOAT );
		REGISTER_ENUM_STRING( ResourceFormat::R8G8B8A8_TYPELESS );
		REGISTER_ENUM_STRING( ResourceFormat::R8G8B8A8_UNORM );
		REGISTER_ENUM_STRING( ResourceFormat::R8G8B8A8_UNORM_SRGB );
		REGISTER_ENUM_STRING( ResourceFormat::R8G8B8A8_UINT );
		REGISTER_ENUM_STRING( ResourceFormat::R8G8B8A8_SNORM );
		REGISTER_ENUM_STRING( ResourceFormat::R8G8B8A8_SINT );
		REGISTER_ENUM_STRING( ResourceFormat::R16G16_TYPELESS );
		REGISTER_ENUM_STRING( ResourceFormat::R16G16_FLOAT );
		REGISTER_ENUM_STRING( ResourceFormat::R16G16_UNORM );
		REGISTER_ENUM_STRING( ResourceFormat::R16G16_UINT );
		REGISTER_ENUM_STRING( ResourceFormat::R16G16_SNORM );
		REGISTER_ENUM_STRING( ResourceFormat::R16G16_SINT );
		REGISTER_ENUM_STRING( ResourceFormat::R32_TYPELESS );
		REGISTER_ENUM_STRING( ResourceFormat::D32_FLOAT );
		REGISTER_ENUM_STRING( ResourceFormat::R32_FLOAT );
		REGISTER_ENUM_STRING( ResourceFormat::R32_UINT );
		REGISTER_ENUM_STRING( ResourceFormat::R32_SINT );
		REGISTER_ENUM_STRING( ResourceFormat::R24G8_TYPELESS );
		REGISTER_ENUM_STRING( ResourceFormat::D24_UNORM_S8_UINT );
		REGISTER_ENUM_STRING( ResourceFormat::R24_UNORM_X8_TYPELESS );
		REGISTER_ENUM_STRING( ResourceFormat::X24_TYPELESS_G8_UINT );
		REGISTER_ENUM_STRING( ResourceFormat::R8G8_TYPELESS );
		REGISTER_ENUM_STRING( ResourceFormat::R8G8_UNORM );
		REGISTER_ENUM_STRING( ResourceFormat::R8G8_UINT );
		REGISTER_ENUM_STRING( ResourceFormat::R8G8_SNORM );
		REGISTER_ENUM_STRING( ResourceFormat::R8G8_SINT );
		REGISTER_ENUM_STRING( ResourceFormat::R16_TYPELESS );
		REGISTER_ENUM_STRING( ResourceFormat::R16_FLOAT );
		REGISTER_ENUM_STRING( ResourceFormat::D16_UNORM );
		REGISTER_ENUM_STRING( ResourceFormat::R16_UNORM );
		REGISTER_ENUM_STRING( ResourceFormat::R16_UINT );
		REGISTER_ENUM_STRING( ResourceFormat::R16_SNORM );
		REGISTER_ENUM_STRING( ResourceFormat::R16_SINT );
		REGISTER_ENUM_STRING( ResourceFormat::R8_TYPELESS );
		REGISTER_ENUM_STRING( ResourceFormat::R8_UNORM );
		REGISTER_ENUM_STRING( ResourceFormat::R8_UINT );
		REGISTER_ENUM_STRING( ResourceFormat::R8_SNORM );
		REGISTER_ENUM_STRING( ResourceFormat::R8_SINT );
		REGISTER_ENUM_STRING( ResourceFormat::A8_UNORM );
		REGISTER_ENUM_STRING( ResourceFormat::R1_UNORM );
		REGISTER_ENUM_STRING( ResourceFormat::R9G9B9E5_SHAREDEXP );
		REGISTER_ENUM_STRING( ResourceFormat::R8G8_B8G8_UNORM );
		REGISTER_ENUM_STRING( ResourceFormat::G8R8_G8B8_UNORM );
		REGISTER_ENUM_STRING( ResourceFormat::BC1_TYPELESS );
		REGISTER_ENUM_STRING( ResourceFormat::BC1_UNORM );
		REGISTER_ENUM_STRING( ResourceFormat::BC1_UNORM_SRGB );
		REGISTER_ENUM_STRING( ResourceFormat::BC2_TYPELESS );
		REGISTER_ENUM_STRING( ResourceFormat::BC2_UNORM );
		REGISTER_ENUM_STRING( ResourceFormat::BC2_UNORM_SRGB );
		REGISTER_ENUM_STRING( ResourceFormat::BC3_TYPELESS );
		REGISTER_ENUM_STRING( ResourceFormat::BC3_UNORM );
		REGISTER_ENUM_STRING( ResourceFormat::BC3_UNORM_SRGB );
		REGISTER_ENUM_STRING( ResourceFormat::BC4_TYPELESS );
		REGISTER_ENUM_STRING( ResourceFormat::BC4_UNORM );
		REGISTER_ENUM_STRING( ResourceFormat::BC4_SNORM );
		REGISTER_ENUM_STRING( ResourceFormat::BC5_TYPELESS );
		REGISTER_ENUM_STRING( ResourceFormat::BC5_UNORM );
		REGISTER_ENUM_STRING( ResourceFormat::BC5_SNORM );
		REGISTER_ENUM_STRING( ResourceFormat::B5G6R5_UNORM );
		REGISTER_ENUM_STRING( ResourceFormat::B5G5R5A1_UNORM );
		REGISTER_ENUM_STRING( ResourceFormat::B8G8R8A8_UNORM );
		REGISTER_ENUM_STRING( ResourceFormat::B8G8R8X8_UNORM );
		REGISTER_ENUM_STRING( ResourceFormat::R10G10B10_XR_BIAS_A2_UNORM );
		REGISTER_ENUM_STRING( ResourceFormat::B8G8R8A8_TYPELESS );
		REGISTER_ENUM_STRING( ResourceFormat::B8G8R8A8_UNORM_SRGB );
		REGISTER_ENUM_STRING( ResourceFormat::B8G8R8X8_TYPELESS );
		REGISTER_ENUM_STRING( ResourceFormat::B8G8R8X8_UNORM_SRGB );
		REGISTER_ENUM_STRING( ResourceFormat::BC6H_TYPELESS );
		REGISTER_ENUM_STRING( ResourceFormat::BC6H_UF16 );
		REGISTER_ENUM_STRING( ResourceFormat::BC6H_SF16 );
		REGISTER_ENUM_STRING( ResourceFormat::BC7_TYPELESS );
		REGISTER_ENUM_STRING( ResourceFormat::BC7_UNORM );
		REGISTER_ENUM_STRING( ResourceFormat::BC7_UNORM_SRGB );

		//Misc Flag
		REGISTER_ENUM_STRING( ResourceMisc::GenerateMips );
		REGISTER_ENUM_STRING( ResourceMisc::Shared );
		REGISTER_ENUM_STRING( ResourceMisc::TextureCube );
		REGISTER_ENUM_STRING( ResourceMisc::DrawIndirectArgs );
		REGISTER_ENUM_STRING( ResourceMisc::BufferAllowRawViews );
		REGISTER_ENUM_STRING( ResourceMisc::BufferStructured );
		REGISTER_ENUM_STRING( ResourceMisc::ResourceClamp );
		REGISTER_ENUM_STRING( ResourceMisc::SharedKeyedMutex );
		REGISTER_ENUM_STRING( ResourceMisc::GdiCompatible );

		// Stencil operation
		REGISTER_ENUM_STRING( StencilOp::Keep );
		REGISTER_ENUM_STRING( StencilOp::Zero );
		REGISTER_ENUM_STRING( StencilOp::Replace );
		REGISTER_ENUM_STRING( StencilOp::IncrSat );
		REGISTER_ENUM_STRING( StencilOp::DecrSat );
		REGISTER_ENUM_STRING( StencilOp::Invert );
		REGISTER_ENUM_STRING( StencilOp::Incr );
		REGISTER_ENUM_STRING( StencilOp::Decr );

		// Texture filter
		REGISTER_ENUM_STRING( TextureFilter::Point );
		REGISTER_ENUM_STRING( TextureFilter::MipLinear );
		REGISTER_ENUM_STRING( TextureFilter::MagLinear );
		REGISTER_ENUM_STRING( TextureFilter::MinLinear );
		REGISTER_ENUM_STRING( TextureFilter::Anisotropic );
		REGISTER_ENUM_STRING( TextureFilter::Comparison );
		REGISTER_ENUM_STRING( TextureFilter::Minimum );
		REGISTER_ENUM_STRING( TextureFilter::Maximum );

		// Texture address mode
		REGISTER_ENUM_STRING( TextureAddressMode::Wrap );
		REGISTER_ENUM_STRING( TextureAddressMode::Mirror );
		REGISTER_ENUM_STRING( TextureAddressMode::Clamp );
		REGISTER_ENUM_STRING( TextureAddressMode::Border );
		REGISTER_ENUM_STRING( TextureAddressMode::MirrorOnce );
	}
}

AGL_FUNC_DLL void BootUpModules()
{
	RegisterResourceEnumString();

	RegisterFactory<agl::IAgl>( &GetGraphicsApi );
	RegisterFactory<agl::IResourceManager>( &GetResourceManager );

	CreateGraphicsApi();
	CreateResourceManager();
}

AGL_FUNC_DLL void ShutdownModules()
{
	DestoryResourceManager();
	DestoryGraphicsApi();

	UnregisterFactory<agl::IResourceManager>();
	UnregisterFactory<agl::IAgl>();
}