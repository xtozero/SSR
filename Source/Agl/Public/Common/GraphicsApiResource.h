#pragma once

#include "ColorTypes.h"
#include "common.h"
#include "EnumClassFlags.h"
#include "NameTypes.h"
#include "RefCounterType.h"
#include "RefHandle.h"
#include "SizedTypes.h"

#include <atomic>
#include <cstddef>
#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace agl
{
	enum class AglType : uint8
	{
		D3D11 = 0,
		D3D12
	};

	enum class ShaderType : int8
	{
		None = -1,
		VS,
		HS,
		DS,
		GS,
		PS,
		CS,
		Count,
	};

	template <typename T>
	constexpr T MAX_SHADER_TYPE = static_cast<T>( ShaderType::Count );

	static constexpr uint32 MAX_VERTEX_LAYOUT_SIZE = 32;
	static constexpr uint32 MAX_VERTEX_SLOT = 32;
	static constexpr uint32 MAX_RENDER_TARGET = 8;

	inline const char* ToString( ShaderType shaderType )
	{
		const char* shaderTypeStr[] = {
			"VS",
			"HS",
			"DS",
			"GS",
			"PS",
			"CS"
		};

		return shaderTypeStr[static_cast<uint32>( shaderType )];
	}

	class GraphicsApiResource : public RefCounter
	{
	public:
		void Init()
		{
			if ( m_isInitialized == false )
			{
				InitResource();
				m_isInitialized = true;
			}
		}

		void Free()
		{
			if ( m_isInitialized )
			{
				FreeResource();
				m_isInitialized = false;
			}
		}

		bool IsBuffer() const
		{
			return m_isBuffer;
		}

		bool IsTexture() const
		{
			return m_isTexture;
		}

	protected:
		bool m_isBuffer = false;
		bool m_isTexture = false;

		Name m_debugName;

	private:
		virtual void Finalizer() override;

		virtual void InitResource() = 0;
		virtual void FreeResource() = 0;

		bool m_isInitialized = false;
	};

	class DeviceDependantResource : public GraphicsApiResource
	{
	};

	enum class ResourceBindType : uint8
	{
		// from D3D11
		None = 0x00,
		VertexBuffer = 0x01,
		IndexBuffer = 0x02,
		ConstantBuffer = 0x04,
		ShaderResource = 0x08,
		StreamOutput = 0x10,
		RenderTarget = 0x20,
		DepthStencil = 0x40,
		RandomAccess = 0x80,
	};
	ENUM_CLASS_FLAGS( ResourceBindType );

	enum class ResourceMisc : uint16
	{
		None = 0x00,
		// from D3D11
		GenerateMips = 0x01,
		Shared = 0x02,
		TextureCube = 0x04,
		DrawIndirectArgs = 0x08,
		BufferAllowRawViews = 0x10,
		BufferStructured = 0x20,
		ResourceClamp = 0x40,
		SharedKeyedMutex = 0x80,
		GdiCompatible = 0x100,

		// Custom
		Texture3D = 0x200,
		Intermediate = 0x400,
		WithoutViews = Intermediate,
	};
	ENUM_CLASS_FLAGS( ResourceMisc );

	enum class ResourceAccessFlag : uint8
	{
		None = 0x00,
		GpuRead = 0x01,
		GpuWrite = 0x02,
		CpuRead = 0x04,
		CpuWrite = 0x08,

		Default = GpuRead | GpuWrite,
		Upload = GpuRead | CpuWrite,
		Download = GpuRead | GpuWrite | CpuRead | CpuWrite,
	};
	ENUM_CLASS_FLAGS( ResourceAccessFlag );

	enum class ResourceFormat : uint8
	{
		// from D3D11
		Unknown = 0,
		R32G32B32A32_TYPELESS = 1,
		R32G32B32A32_FLOAT = 2,
		R32G32B32A32_UINT = 3,
		R32G32B32A32_SINT = 4,
		R32G32B32_TYPELESS = 5,
		R32G32B32_FLOAT = 6,
		R32G32B32_UINT = 7,
		R32G32B32_SINT = 8,
		R16G16B16A16_TYPELESS = 9,
		R16G16B16A16_FLOAT = 10,
		R16G16B16A16_UNORM = 11,
		R16G16B16A16_UINT = 12,
		R16G16B16A16_SNORM = 13,
		R16G16B16A16_SINT = 14,
		R32G32_TYPELESS = 15,
		R32G32_FLOAT = 16,
		R32G32_UINT = 17,
		R32G32_SINT = 18,
		R32G8X24_TYPELESS = 19,
		D32_FLOAT_S8X24_UINT = 20,
		R32_FLOAT_X8X24_TYPELESS = 21,
		X32_TYPELESS_G8X24_UINT = 22,
		R10G10B10A2_TYPELESS = 23,
		R10G10B10A2_UNORM = 24,
		R10G10B10A2_UINT = 25,
		R11G11B10_FLOAT = 26,
		R8G8B8A8_TYPELESS = 27,
		R8G8B8A8_UNORM = 28,
		R8G8B8A8_UNORM_SRGB = 29,
		R8G8B8A8_UINT = 30,
		R8G8B8A8_SNORM = 31,
		R8G8B8A8_SINT = 32,
		R16G16_TYPELESS = 33,
		R16G16_FLOAT = 34,
		R16G16_UNORM = 35,
		R16G16_UINT = 36,
		R16G16_SNORM = 37,
		R16G16_SINT = 38,
		R32_TYPELESS = 39,
		D32_FLOAT = 40,
		R32_FLOAT = 41,
		R32_UINT = 42,
		R32_SINT = 43,
		R24G8_TYPELESS = 44,
		D24_UNORM_S8_UINT = 45,
		R24_UNORM_X8_TYPELESS = 46,
		X24_TYPELESS_G8_UINT = 47,
		R8G8_TYPELESS = 48,
		R8G8_UNORM = 49,
		R8G8_UINT = 50,
		R8G8_SNORM = 51,
		R8G8_SINT = 52,
		R16_TYPELESS = 53,
		R16_FLOAT = 54,
		D16_UNORM = 55,
		R16_UNORM = 56,
		R16_UINT = 57,
		R16_SNORM = 58,
		R16_SINT = 59,
		R8_TYPELESS = 60,
		R8_UNORM = 61,
		R8_UINT = 62,
		R8_SNORM = 63,
		R8_SINT = 64,
		A8_UNORM = 65,
		R1_UNORM = 66,
		R9G9B9E5_SHAREDEXP = 67,
		R8G8_B8G8_UNORM = 68,
		G8R8_G8B8_UNORM = 69,
		BC1_TYPELESS = 70,
		BC1_UNORM = 71,
		BC1_UNORM_SRGB = 72,
		BC2_TYPELESS = 73,
		BC2_UNORM = 74,
		BC2_UNORM_SRGB = 75,
		BC3_TYPELESS = 76,
		BC3_UNORM = 77,
		BC3_UNORM_SRGB = 78,
		BC4_TYPELESS = 79,
		BC4_UNORM = 80,
		BC4_SNORM = 81,
		BC5_TYPELESS = 82,
		BC5_UNORM = 83,
		BC5_SNORM = 84,
		B5G6R5_UNORM = 85,
		B5G5R5A1_UNORM = 86,
		B8G8R8A8_UNORM = 87,
		B8G8R8X8_UNORM = 88,
		R10G10B10_XR_BIAS_A2_UNORM = 89,
		B8G8R8A8_TYPELESS = 90,
		B8G8R8A8_UNORM_SRGB = 91,
		B8G8R8X8_TYPELESS = 92,
		B8G8R8X8_UNORM_SRGB = 93,
		BC6H_TYPELESS = 94,
		BC6H_UF16 = 95,
		BC6H_SF16 = 96,
		BC7_TYPELESS = 97,
		BC7_UNORM = 98,
		BC7_UNORM_SRGB = 99
	};

	inline uint32 ResourceFormatSize( ResourceFormat format )
	{
		switch ( format )
		{
		case ResourceFormat::Unknown:
			break;
		case ResourceFormat::R32G32B32A32_TYPELESS:
		case ResourceFormat::R32G32B32A32_FLOAT:
		case ResourceFormat::R32G32B32A32_UINT:
		case ResourceFormat::R32G32B32A32_SINT:
			return 16;
			break;
		case ResourceFormat::R32G32B32_TYPELESS:
		case ResourceFormat::R32G32B32_FLOAT:
		case ResourceFormat::R32G32B32_UINT:
		case ResourceFormat::R32G32B32_SINT:
			return 12;
			break;
		case ResourceFormat::R16G16B16A16_TYPELESS:
		case ResourceFormat::R16G16B16A16_FLOAT:
		case ResourceFormat::R16G16B16A16_UNORM:
		case ResourceFormat::R16G16B16A16_UINT:
		case ResourceFormat::R16G16B16A16_SNORM:
		case ResourceFormat::R16G16B16A16_SINT:
		case ResourceFormat::R32G32_TYPELESS:
		case ResourceFormat::R32G32_FLOAT:
		case ResourceFormat::R32G32_UINT:
		case ResourceFormat::R32G32_SINT:
		case ResourceFormat::R32G8X24_TYPELESS:
		case ResourceFormat::D32_FLOAT_S8X24_UINT:
		case ResourceFormat::R32_FLOAT_X8X24_TYPELESS:
		case ResourceFormat::X32_TYPELESS_G8X24_UINT:
			return 8;
			break;
		case ResourceFormat::R10G10B10A2_TYPELESS:
		case ResourceFormat::R10G10B10A2_UNORM:
		case ResourceFormat::R10G10B10A2_UINT:
		case ResourceFormat::R11G11B10_FLOAT:
		case ResourceFormat::R8G8B8A8_TYPELESS:
		case ResourceFormat::R8G8B8A8_UNORM:
		case ResourceFormat::R8G8B8A8_UNORM_SRGB:
		case ResourceFormat::R8G8B8A8_UINT:
		case ResourceFormat::R8G8B8A8_SNORM:
		case ResourceFormat::R8G8B8A8_SINT:
		case ResourceFormat::R16G16_TYPELESS:
		case ResourceFormat::R16G16_FLOAT:
		case ResourceFormat::R16G16_UNORM:
		case ResourceFormat::R16G16_UINT:
		case ResourceFormat::R16G16_SNORM:
		case ResourceFormat::R16G16_SINT:
		case ResourceFormat::R32_TYPELESS:
		case ResourceFormat::D32_FLOAT:
		case ResourceFormat::R32_FLOAT:
		case ResourceFormat::R32_UINT:
		case ResourceFormat::R32_SINT:
		case ResourceFormat::R24G8_TYPELESS:
		case ResourceFormat::D24_UNORM_S8_UINT:
		case ResourceFormat::R24_UNORM_X8_TYPELESS:
		case ResourceFormat::X24_TYPELESS_G8_UINT:
			return 4;
			break;
		case ResourceFormat::R8G8_TYPELESS:
		case ResourceFormat::R8G8_UNORM:
		case ResourceFormat::R8G8_UINT:
		case ResourceFormat::R8G8_SNORM:
		case ResourceFormat::R8G8_SINT:
		case ResourceFormat::R16_TYPELESS:
		case ResourceFormat::R16_FLOAT:
		case ResourceFormat::D16_UNORM:
		case ResourceFormat::R16_UNORM:
		case ResourceFormat::R16_UINT:
		case ResourceFormat::R16_SNORM:
		case ResourceFormat::R16_SINT:
			return 2;
			break;
		case ResourceFormat::R8_TYPELESS:
		case ResourceFormat::R8_UNORM:
		case ResourceFormat::R8_UINT:
		case ResourceFormat::R8_SNORM:
		case ResourceFormat::R8_SINT:
		case ResourceFormat::A8_UNORM:
		case ResourceFormat::R1_UNORM:
			return 1;
			break;
		case ResourceFormat::R9G9B9E5_SHAREDEXP:
		case ResourceFormat::R8G8_B8G8_UNORM:
		case ResourceFormat::G8R8_G8B8_UNORM:
			return 4;
			break;
		case ResourceFormat::BC1_TYPELESS:
			break;
		case ResourceFormat::BC1_UNORM:
			break;
		case ResourceFormat::BC1_UNORM_SRGB:
			break;
		case ResourceFormat::BC2_TYPELESS:
			break;
		case ResourceFormat::BC2_UNORM:
			break;
		case ResourceFormat::BC2_UNORM_SRGB:
			break;
		case ResourceFormat::BC3_TYPELESS:
			break;
		case ResourceFormat::BC3_UNORM:
			break;
		case ResourceFormat::BC3_UNORM_SRGB:
			break;
		case ResourceFormat::BC4_TYPELESS:
			break;
		case ResourceFormat::BC4_UNORM:
			break;
		case ResourceFormat::BC4_SNORM:
			break;
		case ResourceFormat::BC5_TYPELESS:
			break;
		case ResourceFormat::BC5_UNORM:
			break;
		case ResourceFormat::BC5_SNORM:
			break;
		case ResourceFormat::B5G6R5_UNORM:
		case ResourceFormat::B5G5R5A1_UNORM:
			return 2;
			break;
		case ResourceFormat::B8G8R8A8_UNORM:
		case ResourceFormat::B8G8R8X8_UNORM:
		case ResourceFormat::R10G10B10_XR_BIAS_A2_UNORM:
		case ResourceFormat::B8G8R8A8_TYPELESS:
		case ResourceFormat::B8G8R8A8_UNORM_SRGB:
		case ResourceFormat::B8G8R8X8_TYPELESS:
		case ResourceFormat::B8G8R8X8_UNORM_SRGB:
			return 4;
			break;
		case ResourceFormat::BC6H_TYPELESS:
			break;
		case ResourceFormat::BC6H_UF16:
			break;
		case ResourceFormat::BC6H_SF16:
			break;
		case ResourceFormat::BC7_TYPELESS:
			break;
		case ResourceFormat::BC7_UNORM:
			break;
		case ResourceFormat::BC7_UNORM_SRGB:
			break;
		default:
			break;
		}

		assert( "Invalid format" && false );
		return 0;
	}

	enum class ResourcePrimitive : uint8
	{
		Undefined = 0,
		Pointlist,
		Linelist,
		Linestrip,
		Trianglelist,
		TriangleStrip,
		LinelistAdj,
		LinestripAdj,
		TrianglelistAdj,
		TriangleStripAdj,
		ControlPointPatchlist1,
		ControlPointPatchlist2,
		ControlPointPatchlist3,
		ControlPointPatchlist4,
		ControlPointPatchlist5,
		ControlPointPatchlist6,
		ControlPointPatchlist7,
		ControlPointPatchlist8,
		ControlPointPatchlist9,
		ControlPointPatchlist10,
		ControlPointPatchlist11,
		ControlPointPatchlist12,
		ControlPointPatchlist13,
		ControlPointPatchlist14,
		ControlPointPatchlist15,
		ControlPointPatchlist16,
		ControlPointPatchlist17,
		ControlPointPatchlist18,
		ControlPointPatchlist19,
		ControlPointPatchlist20,
		ControlPointPatchlist21,
		ControlPointPatchlist22,
		ControlPointPatchlist23,
		ControlPointPatchlist24,
		ControlPointPatchlist25,
		ControlPointPatchlist26,
		ControlPointPatchlist27,
		ControlPointPatchlist28,
		ControlPointPatchlist29,
		ControlPointPatchlist30,
		ControlPointPatchlist31,
		ControlPointPatchlist32,
	};

	enum class ResourceLockFlag : uint8
	{
		Read = 1,
		Write,
		ReadWrite,
		WriteDiscard,
		WriteNoOverwrite,
	};

	enum class DeviceError : uint8
	{
		None = 0,
		DeviceLost,
	};

	enum class TextureFilter : uint32
	{
		Point = 0x0,
		MipLinear = 0x1,
		MagLinear = 0x4,
		MinLinear = 0x10,
		Anisotropic = 0x55,
		Comparison = 0x80,
		Minimum = 0x100,
		Maximum = 0x180,
	};
	ENUM_CLASS_FLAGS( TextureFilter );

	enum class TextureAddressMode : uint8
	{
		Wrap = 1,
		Mirror,
		Clamp,
		Border,
		MirrorOnce,
	};

	enum class ComparisonFunc : uint8
	{
		Never = 1,
		Less,
		Equal,
		LessEqual,
		Greater,
		NotEqual,
		GreaterEqual,
		Always
	};

	enum class FillMode : uint8
	{
		Wireframe = 1,
		Solid,
	};

	enum class CullMode : uint8
	{
		None = 1,
		Front,
		Back,
	};

	enum class StencilOp : uint8
	{
		Keep = 1,
		Zero,
		Replace,
		IncrSat,
		DecrSat,
		Invert,
		Incr,
		Decr,
	};

	enum class DepthWriteMode : uint8
	{
		Zero = 0,
		All,
	};

	enum class Blend : uint8
	{
		Zero = 1,
		One,
		SrcColor,
		InvSrcColor,
		SrcAlpha,
		InvSrcAlpha,
		DestAlpha,
		InvDestAlpha,
		DestColor,
		InvDestColor,
		SrcAlphaSat,
		BlendFactor,
		InvBlendFactor,
		Src1Color,
		InvSrc1Color,
		Src1Alpha,
		InvSrc1Alpha
	};

	enum class BlendOp : uint8
	{
		Add = 1,
		Subtract,
		ReverseSubtract,
		Min,
		Max
	};

	enum class ColorWriteEnable : uint8
	{
		Red = 1,
		Green = 2,
		Blue = 4,
		Alpha = 8,
		All = ( ( ( Red | Green ) | Blue ) | Alpha )
	};

	enum class ResourceState : uint32
	{
		Common = 0,
		VertexAndConstantBuffer = 0x1,
		Indexbuffer = 0x2,
		RenderTarget = 0x4,
		UnorderedAccess = 0x8,
		DepthWrite = 0x10,
		DepthRead = 0x20,
		NonPixelShaderResource = 0x40,
		PixelShaderResource = 0x80,
		StreamOut = 0x100,
		IndirectArgument = 0x200,
		CopyDest = 0x400,
		CopySource = 0x800,
		ResolveDest = 0x1000,
		ResolveSource = 0x2000,
		RaytracingAccelerationStructure = 0x400000,
		ShadingRateSource = 0x1000000,
		GenericRead = ( ( ( ( ( 0x1 | 0x2 ) | 0x40 ) | 0x80 ) | 0x200 ) | 0x800 ),
		Present = 0,
		Predication = 0x200,
		VideoDecodeRead = 0x10000,
		VideoDecodeWrite = 0x20000,
		VideoProcessRead = 0x40000,
		VideoProecssWrite = 0x80000,
		VideoEncodeRead = 0x200000,
		VideoEncodeWrite = 0x800000
	};
	ENUM_CLASS_FLAGS( ResourceState );

	constexpr uint32 AllSubResource = 0xffffffff;

	class ITransitionable
	{
	public:
		virtual ResourceState GetResourceState() const = 0;
		virtual void SetResourceState( ResourceState state ) = 0;
	};

	struct ResourceTransition
	{
		void* m_pResource;
		ITransitionable* m_pTransitionable;
		uint32 m_subResource;
		ResourceState m_state;
	};

	struct ResourceClearValue
	{
		ResourceFormat m_format = ResourceFormat::Unknown;

		union
		{
			float m_color[4];

			struct DepthStencilClearValue
			{
				float m_depth;
				uint8 m_stencil;
			} m_depthStencil;
		};
	};

	struct BufferTrait
	{
		uint32 m_stride;
		uint32 m_count;
		ResourceAccessFlag m_access;
		ResourceBindType m_bindType;
		ResourceMisc m_miscFlag;
		ResourceFormat m_format;
	};

	struct TextureTrait
	{
		uint32 m_width = 0;
		uint32 m_height = 0;
		uint32 m_depth = 0;
		uint32 m_sampleCount = 0;
		uint32 m_sampleQuality = 0;
		uint32 m_mipLevels = 0;
		ResourceFormat m_format = ResourceFormat::Unknown;
		ResourceAccessFlag m_access = ResourceAccessFlag::None;
		ResourceBindType m_bindType = ResourceBindType::None;
		ResourceMisc m_miscFlag = ResourceMisc::None;
		std::optional<ResourceClearValue> m_clearValue;

		AGL_DLL size_t GetHash() const;
	};

	struct ResourceSectionData
	{
		size_t m_offset;
		uint64 m_pitch;
		uint64 m_slicePitch;
	};

	struct ResourceInitData
	{
		void* m_srcData = nullptr;
		size_t m_srcSize = 0;
		std::vector<ResourceSectionData> m_sections;
	};

	struct LockedResource
	{
		void* m_data;
		uint64 m_rowPitch;
		uint64 m_depthPitch;
	};

	struct MultiSampleOption
	{
		int32 m_count;
		int32 m_quality;
	};

	struct SamplerStateTrait
	{
		TextureFilter m_filter;
		TextureAddressMode m_addressU;
		TextureAddressMode m_addressV;
		TextureAddressMode m_addressW;
		float m_mipLODBias;
		uint32 m_maxAnisotropy;
		ComparisonFunc m_comparisonFunc;
		Color m_borderColor;
		float m_minLOD;
		float m_maxLOD;
	};

	struct RasterizerStateTrait
	{
		FillMode m_fillMode;
		CullMode m_cullMode;
		bool m_frontCounterClockwise;
		int32 m_depthBias;
		float m_depthBiasClamp;
		float m_slopeScaleDepthBias;
		bool m_depthClipEnable;
		bool m_scissorEnable;
		bool m_multisampleEnable;
		bool m_antialiasedLineEnable;
	};

	struct RenderTargetBlendTrait
	{
		bool m_blendEnable;
		Blend m_srcBlend;
		Blend m_destBlend;
		BlendOp m_blendOp;
		Blend m_srcBlendAlpha;
		Blend m_destBlendAlpha;
		BlendOp m_blendOpAlpha;
		ColorWriteEnable m_renderTargetWriteMask;
	};

	struct BlendStateTrait
	{
		bool m_alphaToConverageEnable;
		bool m_independentBlendEnable;
		RenderTargetBlendTrait m_renderTarget[MAX_RENDER_TARGET];
		uint32 m_sampleMask;
	};

	struct StencilOpTrait
	{
		StencilOp m_failOp;
		StencilOp m_depthFailOp;
		StencilOp m_passOp;
		ComparisonFunc m_func;

		friend bool operator==( const StencilOpTrait& lhs, const StencilOpTrait& rhs )
		{
			return lhs.m_failOp == rhs.m_failOp
				&& lhs.m_depthFailOp == rhs.m_depthFailOp
				&& lhs.m_passOp == rhs.m_passOp
				&& lhs.m_func == rhs.m_func;
		}
	};

	struct DepthStencilStateTrait
	{
		bool m_depthEnable;
		DepthWriteMode m_depthWriteMode;
		ComparisonFunc m_depthFunc;
		bool m_stencilEnable;
		unsigned char m_stencilReadMask;
		unsigned char m_stencilWriteMask;
		StencilOpTrait m_frontFace;
		StencilOpTrait m_backFace;
	};

	struct VertexLayoutTrait
	{
		bool m_isInstanceData = false;
		uint32 m_index = 0;
		ResourceFormat m_format = ResourceFormat::Unknown;
		uint32 m_slot = 0;
		uint32 m_instanceDataStep = 0;
		Name m_name;

		friend bool operator==( const VertexLayoutTrait& lhs, const VertexLayoutTrait& rhs )
		{
			return  lhs.m_isInstanceData == rhs.m_isInstanceData
				&& lhs.m_index == rhs.m_index
				&& lhs.m_format == rhs.m_format
				&& lhs.m_slot == rhs.m_slot
				&& lhs.m_instanceDataStep == rhs.m_instanceDataStep
				&& lhs.m_name == rhs.m_name;
		}

		friend bool operator!=( const VertexLayoutTrait& lhs, const VertexLayoutTrait& rhs )
		{
			return !( lhs == rhs );
		}
	};
}