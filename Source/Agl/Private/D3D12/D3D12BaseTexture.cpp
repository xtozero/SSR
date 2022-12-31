#include "D3D12BaseTexture.h"

#include "D3D12FlagConvertor.h"
#include "D3D12ResourceViews.h"

using ::agl::ConvertDxgiFormatToFormat;
using ::agl::ConvertFormatToDxgiFormat;
using ::agl::ConvertHeapTypeToAccessFlag;
using ::agl::ConvertResourceFlagsToBindType;
using ::agl::D3D12HeapProperties;
using ::agl::ResourceAccessFlag;
using ::agl::ResourceBindType;
using ::agl::ResourceFormat;
using ::agl::ResourceMisc;
using ::agl::TEXTURE_TRAIT;

namespace
{
	D3D12_RESOURCE_DIMENSION GetResourceDimension( const TEXTURE_TRAIT& trait )
	{
		if ( trait.m_miscFlag == ResourceMisc::Texture3D )
		{
			return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		}
		else if ( trait.m_height > 1 )
		{
			return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		}
		else
		{
			return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
		}
	}

	D3D12_RESOURCE_FLAGS GetResourceFlags( const TEXTURE_TRAIT& trait )
	{
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

		if ( HasAnyFlags( trait.m_bindType, ResourceBindType::RenderTarget ) )
		{
			flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		}

		if ( HasAnyFlags( trait.m_bindType, ResourceBindType::DepthStencil ) )
		{
			flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
			if ( HasAnyFlags( trait.m_bindType, ResourceBindType::ShaderResource ) == false )
			{
				flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
			}
		}

		if ( HasAnyFlags( trait.m_bindType, ResourceBindType::RandomAccess ) )
		{
			flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}

		return flags;
	}

	D3D12_RESOURCE_DESC ConvertTraitToDesc( const TEXTURE_TRAIT& trait )
	{
		uint64 alignment = ( trait.m_sampleCount > 1 ) ? D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT : D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;

		D3D12_RESOURCE_DESC desc = {
			.Dimension = GetResourceDimension(trait),
			.Alignment = alignment,
			.Width = trait.m_width,
			.Height = trait.m_height,
			.DepthOrArraySize = static_cast<uint16>( trait.m_depth ),
			.MipLevels = static_cast<uint16>( trait.m_mipLevels ),
			.Format = ConvertFormatToDxgiFormat( trait.m_format ),
			.SampleDesc = {
				.Count = trait.m_sampleCount,
				.Quality = trait.m_sampleQuality
			},
			.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
			.Flags = GetResourceFlags( trait )
		};

		return desc;
	}

	TEXTURE_TRAIT ConvertDescToTrait( const D3D12_RESOURCE_DESC& desc, const D3D12_HEAP_PROPERTIES& heapProperties )
	{
		ResourceFormat format = ConvertDxgiFormatToFormat( desc.Format );
		ResourceBindType bindType = ConvertResourceFlagsToBindType( desc.Flags );
		ResourceAccessFlag access = ConvertHeapTypeToAccessFlag( heapProperties.Type );

		return TEXTURE_TRAIT{
			.m_width = static_cast<uint32>( desc.Width ),
			.m_height = static_cast<uint32>( desc.Height ),
			.m_depth = static_cast<uint32>( desc.DepthOrArraySize ),
			.m_sampleCount = desc.SampleDesc.Count,
			.m_sampleQuality = desc.SampleDesc.Quality,
			.m_mipLevels = desc.MipLevels,
			.m_format = format,
			.m_access = access,
			.m_bindType = bindType,
			.m_miscFlag = ResourceMisc::None
		};
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC ConvertTraitToNonMultiSampleSRV( const TEXTURE_TRAIT& trait )
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srv = {};
		srv.Format = ConvertFormatToDxgiFormat( trait.m_format );
		srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		if ( HasAnyFlags( trait.m_miscFlag, ResourceMisc::Texture3D ) )
		{
			srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
			srv.Texture3D = {
				.MostDetailedMip = 0,
				.MipLevels = trait.m_mipLevels,
				.ResourceMinLODClamp = 0
			};
		}
		else if ( trait.m_height > 1 )
		{
			if ( HasAnyFlags( trait.m_miscFlag, ResourceMisc::TextureCube ) )
			{
				assert( ( trait.m_depth % 6 == 0 ) && "texture cube's depth count must be multiples of 6" );
				if ( trait.m_depth == 6 )
				{
					srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
					srv.TextureCube = {
						.MostDetailedMip = 0,
						.MipLevels = trait.m_mipLevels,
						.ResourceMinLODClamp = 0
					};
				}
				else
				{
					srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
					srv.TextureCubeArray = {
						.MostDetailedMip = 0,
						.MipLevels = trait.m_mipLevels,
						.First2DArrayFace = 0,
						.NumCubes = trait.m_depth / 6,
						.ResourceMinLODClamp = 0
					};
				}
			}
			else
			{
				if ( trait.m_depth <= 1 )
				{
					srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
					srv.Texture2D = {
						.MostDetailedMip = 0,
						.MipLevels = trait.m_mipLevels,
						.PlaneSlice = 0,
						.ResourceMinLODClamp = 0
					};
				}
				else
				{
					srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
					srv.Texture2DArray = {
						.MostDetailedMip = 0,
						.MipLevels = trait.m_mipLevels,
						.FirstArraySlice = 0,
						.ArraySize = trait.m_depth,
						.PlaneSlice = 0,
						.ResourceMinLODClamp = 0
					};
				}
			}
		}
		else
		{
			if ( trait.m_depth == 1 )
			{
				srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
				srv.Texture1D = {
					.MostDetailedMip = 0,
					.MipLevels = trait.m_mipLevels,
					.ResourceMinLODClamp = 0
				};
			}
			else
			{
				srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
				srv.Texture1DArray = {
					.MostDetailedMip = 0,
					.MipLevels = trait.m_mipLevels,
					.FirstArraySlice = 0,
					.ArraySize = trait.m_depth,
					.ResourceMinLODClamp = 0
				};
			}
		}

		return srv;
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC ConvertTraitToMultiSampleSRV( const TEXTURE_TRAIT& trait )
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srv = {};
		srv.Format = ConvertFormatToDxgiFormat( trait.m_format );
		srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		assert( ( HasAnyFlags( trait.m_miscFlag, ResourceMisc::Texture3D ) == false )
			&& "texture 3d can't be multi sampled" );
		assert( ( trait.m_height > 1 )
			&& "texture 1d can't be multi sampled" );

		if ( trait.m_depth <= 1 )
		{
			srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
			srv.Texture2DMS = {};
		}
		else
		{
			srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
			srv.Texture2DMSArray = {
				.FirstArraySlice = 0,
				.ArraySize = trait.m_depth
			};
		}

		return srv;
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC ConvertTraitToSRV( const TEXTURE_TRAIT& trait )
	{
		if ( trait.m_sampleCount <= 1 )
		{
			return ConvertTraitToNonMultiSampleSRV( trait );
		}
		else
		{
			return ConvertTraitToMultiSampleSRV( trait );
		}
	}

	D3D12_UNORDERED_ACCESS_VIEW_DESC ConvertTraitToUAV( const TEXTURE_TRAIT& trait )
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC uav = {
			.Format = ConvertFormatToDxgiFormat( trait.m_format )
		};

		if ( HasAnyFlags( trait.m_miscFlag, ResourceMisc::Texture3D ) )
		{
			uav.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
			uav.Texture3D = {
				.MipSlice = 0,
				.FirstWSlice = 0,
				.WSize = trait.m_depth
			};
		}
		else if ( trait.m_height > 1 )
		{
			if ( trait.m_depth > 1 )
			{
				uav.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
				uav.Texture2DArray = {
					.MipSlice = 0,
					.FirstArraySlice = 0,
					.ArraySize = trait.m_depth,
					.PlaneSlice = 0
				};
			}
			else
			{
				uav.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
				uav.Texture2D = {
					.MipSlice = 0,
					.PlaneSlice = 0
				};
			}
		}
		else
		{
			if ( trait.m_depth > 1 )
			{
				uav.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
				uav.Texture1DArray = {
					.MipSlice = 0,
					.FirstArraySlice = 0,
					.ArraySize = trait.m_depth
				};
			}
			else
			{
				uav.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
				uav.Texture1D = {
					.MipSlice = 0
				};
			}
		}

		return uav;
	}

	D3D12_RENDER_TARGET_VIEW_DESC ConvertTraitToNonMultiSampleRTV( const TEXTURE_TRAIT& trait )
	{
		D3D12_RENDER_TARGET_VIEW_DESC rtv = {
			.Format = ConvertFormatToDxgiFormat( trait.m_format )
		};

		if ( HasAnyFlags( trait.m_miscFlag, ResourceMisc::Texture3D ) )
		{
			rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
			rtv.Texture3D = {
				.MipSlice = 0,
				.FirstWSlice = 0,
				.WSize = trait.m_depth
			};
		}
		else if ( trait.m_height > 1 )
		{
			if ( trait.m_depth > 1 )
			{
				rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
				rtv.Texture2DArray = {
					.MipSlice = 0,
					.FirstArraySlice = 0,
					.ArraySize = trait.m_depth,
					.PlaneSlice = 0
				};
			}
			else
			{
				rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
				rtv.Texture2D = {
					.MipSlice = 0,
					.PlaneSlice = 0
				};
			}
		}
		else
		{
			if ( trait.m_depth > 1 )
			{
				rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
				rtv.Texture1DArray = {
					.MipSlice = 0,
					.FirstArraySlice = 0,
					.ArraySize = trait.m_depth
				};
			}
			else
			{
				rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
				rtv.Texture1D = {
					.MipSlice = 0
				};
			}
		}

		return rtv;
	}

	D3D12_RENDER_TARGET_VIEW_DESC ConvertTraitToMultiSampleRTV( const TEXTURE_TRAIT& trait )
	{
		D3D12_RENDER_TARGET_VIEW_DESC rtv = {
			.Format = ConvertFormatToDxgiFormat( trait.m_format )
		};

		assert( ( HasAnyFlags( trait.m_miscFlag, ResourceMisc::Texture3D ) == false )
			&& "texture 3d can't be multi sampled" );
		assert( ( trait.m_height > 1 )
			&& "texture 1d can't be multi sampled" );

		if ( trait.m_depth > 1 )
		{
			rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
			rtv.Texture2DMSArray = {
				.FirstArraySlice = 0,
				.ArraySize = trait.m_depth
			};
		}
		else
		{
			rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
			rtv.Texture2DMS = {};
		}

		return rtv;
	}

	D3D12_RENDER_TARGET_VIEW_DESC ConvertTraitToRTV( const TEXTURE_TRAIT& trait )
	{
		if ( trait.m_sampleCount > 1 )
		{
			return ConvertTraitToMultiSampleRTV( trait );
		}
		else
		{
			return ConvertTraitToNonMultiSampleRTV( trait );
		}
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC ConvertTraitToNonMultiSampleDSV( const TEXTURE_TRAIT& trait )
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {
			.Format = ConvertFormatToDxgiFormat( trait.m_format )
		};

		assert( HasAnyFlags( trait.m_miscFlag, ResourceMisc::Texture3D ) == false );
		if ( trait.m_height > 1 )
		{
			if ( trait.m_depth > 1 )
			{
				dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
				dsv.Texture2DArray = {
					.MipSlice = 0,
					.FirstArraySlice = 0,
					.ArraySize = trait.m_depth
				};
			}
			else
			{
				dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				dsv.Texture2D = {
					.MipSlice = 0
				};
			}
		}
		else
		{
			if ( trait.m_depth > 1 )
			{
				dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
				dsv.Texture1DArray = {
					.MipSlice = 0,
					.FirstArraySlice = 0,
					.ArraySize = trait.m_depth
				};
			}
			else
			{
				dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;
				dsv.Texture1D = {
					.MipSlice = 0
				};
			}
		}

		return dsv;
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC ConvertTraitToMultiSampleDSV( const TEXTURE_TRAIT& trait )
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {
			.Format = ConvertFormatToDxgiFormat( trait.m_format )
		};

		assert( ( HasAnyFlags( trait.m_miscFlag, ResourceMisc::Texture3D ) == false )
			&& "texture 3d can't be multi sampled" );
		assert( ( trait.m_height > 1 )
			&& "texture 1d can't be multi sampled" );

		if ( trait.m_depth > 1 )
		{
			dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
			dsv.Texture2DMSArray = {
				.FirstArraySlice = 0,
				.ArraySize = trait.m_depth
			};
		}
		else
		{
			dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
			dsv.Texture2DMS = {};
		}

		return dsv;
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC ConvertTraitToDSV( const TEXTURE_TRAIT& trait )
	{
		if ( trait.m_sampleCount > 1 )
		{
			return ConvertTraitToMultiSampleDSV( trait );
		}
		else
		{
			return ConvertTraitToNonMultiSampleDSV( trait );
		}
	}

	D3D12HeapProperties ConvertToHeapProperties( const TEXTURE_TRAIT& trait )
	{
		uint64 alignment = ( trait.m_sampleCount > 1 ) ? D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT : D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;

		D3D12HeapProperties properties = {
			.m_alignment = alignment,
			.m_heapType = ConvertAccessFlagToHeapType( trait.m_access ),
			.m_heapFlags = D3D12_HEAP_FLAG_NONE
		};

		return properties;
	}

	D3D12_RESOURCE_STATES ConvertToStates( [[maybe_unused]] const TEXTURE_TRAIT& trait )
	{
		D3D12_RESOURCE_STATES states = D3D12_RESOURCE_STATE_COMMON;
		return states;
	}
}

namespace agl
{
	D3D12Texture::D3D12Texture( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
		: TextureBase( trait, initData )
		, m_desc( ConvertTraitToDesc( trait ) )
	{
		if ( initData )
		{
			size_t numSections = initData->m_sections.size();

			m_initData.resize( numSections );
			for ( size_t i = 0; i < numSections; ++i )
			{
				const RESOURCE_SECTION_DATA& section = initData->m_sections[i];

				m_initData[i].pData = static_cast<unsigned char*>( m_dataStorage ) + section.m_offset;
				m_initData[i].RowPitch = section.m_pitch;
				m_initData[i].SlicePitch = section.m_slicePitch;
			}
		}
	}

	void D3D12Texture::CreateTexture()
	{
		D3D12HeapProperties properties = ConvertToHeapProperties( m_trait );
		D3D12_RESOURCE_STATES states = ConvertToStates( m_trait );
		if ( ( m_dataStorage != nullptr ) && ( properties.m_heapType != D3D12_HEAP_TYPE_UPLOAD ) )
		{
			states = D3D12_RESOURCE_STATE_COPY_DEST;
		}

		D3D12ResourceAllocator& allocator = D3D12ResourceAllocator::GetInstance();
		m_resourceInfo = allocator.AllocateResource(
			properties,
			m_desc,
			states
		);
	}

	void D3D12Texture::CreateShaderResource()
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = ConvertTraitToSRV( m_trait );
		m_srv = new D3D12ShaderResourceView( this, static_cast<ID3D12Resource*>( Resource() ), srvDesc );
		m_srv->Init();
	}

	void D3D12Texture::CreateUnorderedAccess()
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = ConvertTraitToUAV( m_trait );
		m_uav = new D3D12UnorderedAccessView( this, static_cast<ID3D12Resource*>( Resource() ), uavDesc );
		m_uav->Init();
	}

	D3D12BaseTexture1D::D3D12BaseTexture1D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
		: D3D12Texture( trait, initData )
	{
	}

	D3D12BaseTexture2D::D3D12BaseTexture2D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
		: D3D12Texture( trait, initData )
	{
	}

	D3D12BaseTexture2D::D3D12BaseTexture2D( ID3D12Resource* texture, const D3D12_RESOURCE_DESC* desc )
	{
		if ( texture )
		{
			m_resourceInfo.m_resource = texture;
			if ( desc == nullptr )
			{
				m_desc = texture->GetDesc();
			}
			else
			{
				m_desc = *desc;
			}

			HRESULT hr = texture->GetHeapProperties( &m_heapProperties, &m_heapFlags );
			assert( SUCCEEDED( hr ) );

			m_trait = ConvertDescToTrait( m_desc, m_heapProperties );
		}
	}

	void D3D12BaseTexture2D::CreateRenderTarget()
	{
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = ConvertTraitToRTV( m_trait );
		m_rtv = new D3D12RenderTargetView( this, static_cast<ID3D12Resource*>( Resource() ), rtvDesc );
		m_rtv->Init();
	}

	void D3D12BaseTexture2D::CreateDepthStencil()
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = ConvertTraitToDSV( m_trait );
		m_dsv = new D3D12DepthStencilView( this, static_cast<ID3D12Resource*>( Resource() ), dsvDesc );
		m_dsv->Init();
	}

	D3D12BaseTexture3D::D3D12BaseTexture3D( const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData )
		: D3D12Texture( trait, initData )
	{
	}
}
