#pragma once

#include "common.h"
#include "DataStructure/EnumStringMap.h"
#include "RefHandle.h"

#include <atomic>
#include <cstddef>
#include <limits>

enum class SHADER_TYPE
{
	NONE = -1,
	VS,
	HS,
	DS,
	GS,
	PS,
	CS,
	Count,
};

template <typename T>
constexpr T MAX_SHADER_TYPE = static_cast<T>( SHADER_TYPE::Count );

inline constexpr char* ToString( SHADER_TYPE shaderType )
{
	constexpr char* shaderTypeStr[] = {
		"VS",
		"HS",
		"DS",
		"GS",
		"PS",
		"CS"
	};

	return shaderTypeStr[static_cast<int>( shaderType )];
}

// using Material = std::size_t;
// constexpr Material INVALID_MATERIAL = std::numeric_limits<std::size_t>::max();

enum class GraphicsResourceType
{
	BUFFER = 0,
	TEXTURE1D,
	TEXTURE2D,
	TEXTURE3D,
	SHADER_RESOURCE,
	RANDOM_ACCESS,
	RENDER_TARGET,
	DEPTH_STENCIL,
	VERTEX_LAYOUT,
	VERTEX_SHADER,
	HULL_SHADER,
	DOMAIN_SHADER,
	GEOMETRY_SHADER,
	PIXEL_SHADER,
	COMPUTE_SHADER,
	SAMPLER_STATE,
	RASTERIZER_STATE,
	BLEND_STATE,
	DEPTH_STENCIL_STATE,
	COUNT,
};

class GraphicsApiResource
{
public:
	virtual ~GraphicsApiResource( ) = default;

	virtual void Free( ) = 0;
	virtual void InitResource( ) = 0;

	AGA_DLL int AddRef( );
	AGA_DLL int ReleaseRef( );

private:
	std::atomic<int> m_refCount = 0;
};

class DeviceDependantResource : public GraphicsApiResource
{
};

struct RE_HANDLE
{
	GraphicsResourceType m_type;
	RefHandle<GraphicsApiResource> m_resource;

	bool IsValid() const
	{
		return ( m_type != GraphicsResourceType::COUNT ) && ( m_resource.Get( ) != nullptr );
	}

	RE_HANDLE( ) : m_type( GraphicsResourceType::COUNT ) {}

	RE_HANDLE( GraphicsResourceType type, GraphicsApiResource* resource ) : m_type( type ), m_resource( resource )
	{}

	friend bool operator==( const RE_HANDLE& lhs, const RE_HANDLE& rhs )
	{
		return lhs.m_type == rhs.m_type && lhs.m_resource == rhs.m_resource;
	}

	friend bool operator!=( const RE_HANDLE& lhs, const RE_HANDLE& rhs )
	{
		return !( lhs == rhs );
	}
};

inline bool IsBufferHandle( RE_HANDLE handle )
{
	return handle.m_type == GraphicsResourceType::BUFFER;
}

inline bool IsDepthStencilHandle( RE_HANDLE handle )
{
	return handle.m_type == GraphicsResourceType::DEPTH_STENCIL;
}

inline bool IsRenderTargetHandle( RE_HANDLE handle )
{
	return handle.m_type == GraphicsResourceType::RENDER_TARGET;
}

inline bool IsShaderResourceHandle( RE_HANDLE handle )
{
	return handle.m_type == GraphicsResourceType::SHADER_RESOURCE;
}

inline bool IsRandomAccessHandle( RE_HANDLE handle )
{
	return handle.m_type == GraphicsResourceType::RANDOM_ACCESS;
}

inline bool IsTexture1DHandle( RE_HANDLE handle )
{
	return handle.m_type == GraphicsResourceType::TEXTURE1D;
}

inline bool IsTexture2DHandle( RE_HANDLE handle )
{
	return handle.m_type == GraphicsResourceType::TEXTURE1D;
}

inline bool IsTexture3DHandle( RE_HANDLE handle )
{
	return handle.m_type == GraphicsResourceType::TEXTURE1D;
}

inline bool IsVertexLayout( RE_HANDLE handle )
{
	return handle.m_type == GraphicsResourceType::VERTEX_LAYOUT;
}

inline bool IsVertexShaderHandle( RE_HANDLE handle )
{
	return handle.m_type == GraphicsResourceType::VERTEX_SHADER;
}

inline bool IsGeometryShaderHandle( RE_HANDLE handle )
{
	return handle.m_type == GraphicsResourceType::GEOMETRY_SHADER;
}

inline bool IsPixelShaderHandle( RE_HANDLE handle )
{
	return handle.m_type == GraphicsResourceType::PIXEL_SHADER;
}

inline bool IsComputeShaderHandle( RE_HANDLE handle )
{
	return handle.m_type == GraphicsResourceType::COMPUTE_SHADER;
}

inline bool IsSamplerStateHandle( RE_HANDLE handle )
{
	return handle.m_type == GraphicsResourceType::SAMPLER_STATE;
}

inline bool IsRasterizerStateHandle( RE_HANDLE handle )
{
	return handle.m_type == GraphicsResourceType::RASTERIZER_STATE;
}

inline bool IsBlendStateHandle( RE_HANDLE handle )
{
	return handle.m_type == GraphicsResourceType::BLEND_STATE;
}

inline bool IsDepthStencilStateHandle( RE_HANDLE handle )
{
	return handle.m_type == GraphicsResourceType::DEPTH_STENCIL_STATE;
}

namespace RESOURCE_BIND_TYPE
{
	enum
	{
		// from D3D11
		VERTEX_BUFFER = 0x01,
		INDEX_BUFFER = 0x02,
		CONSTANT_BUFFER = 0x04,
		SHADER_RESOURCE = 0x08,
		STREAM_OUTPUT = 0x10,
		RENDER_TARGET = 0x20,
		DEPTH_STENCIL = 0x40,
		RANDOM_ACCESS = 0x80,
	};
}

namespace RESOURCE_MISC
{
	enum
	{
		NONE = 0x00,
		// from D3D11
		GENERATE_MIPS = 0x01,
		SHARED = 0x02,
		TEXTURECUBE = 0x04,
		DRAWINDIRECT_ARGS = 0x08,
		BUFFER_ALLOW_RAW_VIEWS = 0x10,
		BUFFER_STRUCTURED = 0x20,
		RESOURCE_CLAMP = 0x40,
		SHARED_KEYEDMUTEX = 0x80,
		GDI_COMPATIBLE = 0x100,

		// Custom
		APP_SIZE_DEPENDENT = 0x200,
	};
}

namespace RESOURCE_ACCESS_FLAG
{
	enum
	{
		NONE = 0x00,
		GPU_READ = 0x01,
		GPU_WRITE = 0x02,
		CPU_READ = 0x04,
		CPU_WRITE = 0x08,
	};
}

enum class RESOURCE_FORMAT
{
	// from D3D11
	UNKNOWN = 0,
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

enum class RESOURCE_PRIMITIVE
{
	UNDEFINED = 0,
	POINTLIST,
	LINELIST,
	LINESTRIP,
	TRIANGLELIST,
	TRIANGLESTRIP,
	LINELIST_ADJ,
	LINESTRIP_ADJ,
	TRIANGLELIST_ADJ,
	TRIANGLESTRIP_ADJ,
	CONTROL_POINT_PATCHLIST_1,
	CONTROL_POINT_PATCHLIST_2,
	CONTROL_POINT_PATCHLIST_3,
	CONTROL_POINT_PATCHLIST_4,
	CONTROL_POINT_PATCHLIST_5,
	CONTROL_POINT_PATCHLIST_6,
	CONTROL_POINT_PATCHLIST_7,
	CONTROL_POINT_PATCHLIST_8,
	CONTROL_POINT_PATCHLIST_9,
	CONTROL_POINT_PATCHLIST_10,
	CONTROL_POINT_PATCHLIST_11,
	CONTROL_POINT_PATCHLIST_12,
	CONTROL_POINT_PATCHLIST_13,
	CONTROL_POINT_PATCHLIST_14,
	CONTROL_POINT_PATCHLIST_15,
	CONTROL_POINT_PATCHLIST_16,
	CONTROL_POINT_PATCHLIST_17,
	CONTROL_POINT_PATCHLIST_18,
	CONTROL_POINT_PATCHLIST_19,
	CONTROL_POINT_PATCHLIST_20,
	CONTROL_POINT_PATCHLIST_21,
	CONTROL_POINT_PATCHLIST_22,
	CONTROL_POINT_PATCHLIST_23,
	CONTROL_POINT_PATCHLIST_24,
	CONTROL_POINT_PATCHLIST_25,
	CONTROL_POINT_PATCHLIST_26,
	CONTROL_POINT_PATCHLIST_27,
	CONTROL_POINT_PATCHLIST_28,
	CONTROL_POINT_PATCHLIST_29,
	CONTROL_POINT_PATCHLIST_30,
	CONTROL_POINT_PATCHLIST_31,
	CONTROL_POINT_PATCHLIST_32,
};

namespace BUFFER_LOCKFLAG
{
	enum
	{
		READ = 1,
		WRITE,
		READ_WRITE,
		WRITE_DISCARD,
		WRITE_NO_OVERWRITE,
	};
}

namespace DEVICE_ERROR
{
	enum
	{
		NONE = 0,
		DEVICE_LOST,
	};
}

namespace TEXTURE_FILTER
{
	enum Type
	{
		POINT = 0x0,
		MIP_LINEAR = 0x1,
		MAG_LINEAR = 0x4,
		MIN_LINEAR = 0x10,
		ANISOTROPIC = 0x55,
		COMPARISON = 0x80,
		MINIMUM = 0x100,
		MAXIMUM = 0x180,
	};
}

enum class TEXTURE_ADDRESS_MODE
{
	WRAP = 1,
	MIRROR,
	CLAMP,
	BORDER,
	MIRROR_ONCE,
};

enum class COMPARISON_FUNC
{
	NEVER = 1,
	LESS,
	EQUAL,
	LESS_EQUAL,
	GREATER,
	NOT_EQUAL,
	GREATER_EQUAL,
	ALWAYS
};

enum class FILL_MODE
{
	WIREFRAME = 1,
	SOLID,
};

enum class CULL_MODE
{
	NONE = 1,
	FRONT,
	BACK,
};

enum class STENCIL_OP
{
	KEEP = 1,
	ZERO,
	REPLACE,
	INCR_SAT,
	DECR_SAT,
	INVERT,
	INCR,
	DECR,
};

enum class DEPTH_WRITE_MODE
{
	ZERO = 0,
	ALL,
};

enum class BLEND
{
	ZERO = 1,
	ONE,
	SRC_COLOR,
	INV_SRC_COLOR,
	SRC_ALPHA,
	INV_SRC_ALPHA,
	DEST_ALPHA,
	INV_DEST_ALPHA,
	DEST_COLOR,
	INV_DEST_COLOR,
	SRC_ALPHA_SAT,
	BLEND_FACTOR,
	INV_BLEND_FACTOR,
	SRC1_COLOR,
	INV_SRC1_COLOR,
	SRC1_ALPHA,
	INV_SRC1_ALPHA
};

enum class BLEND_OP
{
	ADD = 1,
	SUBTRACT,
	REV_SUBTRACT,
	MIN,
	MAX
};

enum class COLOR_WRITE_ENABLE
{
	RED = 1,
	GREEN = 2,
	BLUE = 4,
	ALPHA = 8,
	ALL = ( ( ( RED | GREEN ) | BLUE ) | ALPHA )
};

struct BUFFER_TRAIT
{
	UINT m_stride;
	UINT m_count;
	UINT m_access;
	UINT m_bindType;
	UINT m_miscFlag;
};

struct TEXTURE_TRAIT
{
	UINT m_width;
	UINT m_height;
	UINT m_depth;
	UINT m_sampleCount;
	UINT m_sampleQuality;
	UINT m_mipLevels;
	RESOURCE_FORMAT m_format;
	UINT m_access;
	UINT m_bindType;
	UINT m_miscFlag;
};

struct RESOURCE_INIT_DATA
{
	void* m_srcData;
	std::size_t m_srcSize;
	UINT m_pitch;
	UINT m_slicePitch;
};

struct Viewport
{
	float m_x;
	float m_y;
	float m_width;
	float m_height;
	float m_near;
	float m_far;
};

struct RESOURCE_REGION
{
	UINT m_subResource;
	UINT m_left;
	UINT m_right;
	UINT m_top;
	UINT m_bottom;
	UINT m_front;
	UINT m_back;
};

struct VERTEX_LAYOUT
{
	VERTEX_LAYOUT( const char* name, int index, RESOURCE_FORMAT format, int slot, bool isInstanceData, int instanceDataStep ) : m_name( name ), m_index( index ), m_format( format ), m_slot( slot ), m_isInstanceData( isInstanceData ), m_instanceDataStep( instanceDataStep ) {}

	std::string m_name;
	int m_index;
	RESOURCE_FORMAT m_format;
	int m_slot;
	bool m_isInstanceData;
	int m_instanceDataStep;
};

struct MULTISAMPLE_OPTION
{
	int m_count;
	int m_quality;
};

struct SAMPLER_STATE_TRAIT
{
	TEXTURE_FILTER::Type m_filter;
	TEXTURE_ADDRESS_MODE m_addressU;
	TEXTURE_ADDRESS_MODE m_addressV;
	TEXTURE_ADDRESS_MODE m_addressW;
	float m_mipLODBias;
	UINT m_maxAnisotropy;
	COMPARISON_FUNC m_comparisonFunc;
	float m_borderColor[4];
	float m_minLOD;
	float m_maxLOD;
};

struct RASTERIZER_STATE_TRAIT
{
	FILL_MODE m_fillMode;
	CULL_MODE m_cullMode;
	bool m_frontCounterClockwise;
	int m_depthBias;
	float m_depthBiasClamp;
	float m_slopeScaleDepthBias;
	bool m_depthClipEnable;
	bool m_scissorEnable;
	bool m_multisampleEnalbe;
	bool m_antialiasedLineEnable;
};

struct RENDER_TARGET_BLEND_TRAIT
{
	bool m_blendEnable;
	BLEND m_srcBlend;
	BLEND m_destBlend;
	BLEND_OP m_blendOp;
	BLEND m_srcBlendAlpha;
	BLEND m_destBlendAlpha;
	BLEND_OP m_blendOpAlpha;
	COLOR_WRITE_ENABLE m_renderTargetWriteMask;
};

struct BLEND_STATE_TRAIT
{
	bool m_alphaToConverageEnable;
	bool m_independentBlendEnable;
	RENDER_TARGET_BLEND_TRAIT m_renderTarget[8];
};

struct STENCIL_OP_TRAIT
{
	STENCIL_OP m_failOp;
	STENCIL_OP m_depthFailOp;
	STENCIL_OP m_passOp;
	COMPARISON_FUNC m_func;
};

struct DEPTH_STENCIL_STATE_TRAIT
{
	bool m_depthEnable;
	DEPTH_WRITE_MODE m_depthWriteMode;
	COMPARISON_FUNC m_depthFunc;
	bool m_stencilEnable;
	unsigned char m_stencilReadMask;
	unsigned char m_stencilWriteMask;
	STENCIL_OP_TRAIT m_frontFace;
	STENCIL_OP_TRAIT m_backFace;
};

struct ShaderStates
{
	RE_HANDLE m_vertexLayout;
	RE_HANDLE m_vertexShader;
	RE_HANDLE m_hullShader;
	RE_HANDLE m_domainShader;
	RE_HANDLE m_geometryShader;
	RE_HANDLE m_pixelShader;
};

struct GraphisPipelineState
{
	ShaderStates m_shaderState;
	RE_HANDLE m_rasterizerState;
	RE_HANDLE m_depthStencilState;
	RE_HANDLE m_blendState;
	RESOURCE_PRIMITIVE m_primitive;
};
