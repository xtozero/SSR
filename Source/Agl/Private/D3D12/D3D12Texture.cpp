#include "D3D12Texture.h"

#include "D3D12Api.h"

#include "D3D12FlagConvertor.h"

#include "D3D12ResourceUploader.h"
#include "D3D12ResourceViews.h"

using ::agl::ConvertDxgiFormatForDSV;
using ::agl::ConvertDxgiFormatForSRV;
using ::agl::ConvertDxgiFormatToFormat;
using ::agl::ConvertFormatToDxgiFormat;
using ::agl::ConvertToResourceAccess;
using ::agl::ConvertResourceFlagsToBindType;
using ::agl::D3D12HeapProperties;
using ::agl::ResourceAccess;
using ::agl::ResourceBindType;
using ::agl::ResourceClearValue;
using ::agl::ResourceFormat;
using ::agl::ResourceMisc;
using ::agl::TextureDesc;

namespace
{
	D3D12_RESOURCE_DIMENSION GetResourceDimension( const TextureDesc& desc )
	{
		if ( HasAnyFlags( desc.m_miscFlag, ResourceMisc::Texture3D ) )
		{
			return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		}
		else
		{
			return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		}
	}

	D3D12_RESOURCE_FLAGS GetResourceFlags( const TextureDesc& desc )
	{
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE;

		if ( HasAnyFlags( desc.m_bindType, ResourceBindType::RenderTarget ) )
		{
			flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		}

		if ( HasAnyFlags( desc.m_bindType, ResourceBindType::DepthStencil ) )
		{
			flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
			if ( HasAnyFlags( desc.m_bindType, ResourceBindType::ShaderResource ) == false )
			{
				flags |= D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE;
			}
		}

		if ( HasAnyFlags( desc.m_bindType, ResourceBindType::RandomAccess ) )
		{
			flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
		}

		return flags;
	}

	D3D12_RESOURCE_DESC ConvertToD3DDesc( const TextureDesc& desc )
	{
		uint64 alignment = ( desc.m_sampleCount > 1 ) ? D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT : D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;

		D3D12_RESOURCE_DESC d3dDesc = {
			.Dimension = GetResourceDimension( desc ),
			.Alignment = alignment,
			.Width = desc.m_width,
			.Height = desc.m_height,
			.DepthOrArraySize = static_cast<uint16>( desc.m_depth ),
			.MipLevels = static_cast<uint16>( desc.m_mipLevels ),
			.Format = ConvertFormatToDxgiFormat( desc.m_format ),
			.SampleDesc = {
				.Count = desc.m_sampleCount,
				.Quality = desc.m_sampleQuality
			},
			.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
			.Flags = GetResourceFlags( desc )
		};

		return d3dDesc;
	}

	TextureDesc ConvertToTextureDesc( const D3D12_RESOURCE_DESC& desc, const D3D12_HEAP_PROPERTIES& heapProperties )
	{
		ResourceFormat format = ConvertDxgiFormatToFormat( desc.Format );
		ResourceBindType bindType = ConvertResourceFlagsToBindType( desc.Flags );
		ResourceAccess access = ConvertToResourceAccess( heapProperties.Type );

		return TextureDesc{
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

	D3D12_SHADER_RESOURCE_VIEW_DESC ConvertToNonMultiSampleSRVDesc( const TextureDesc& desc )
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srv = {};
		srv.Format = ConvertDxgiFormatForSRV( ConvertFormatToDxgiFormat( desc.m_format ) );
		srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		if ( HasAnyFlags( desc.m_miscFlag, ResourceMisc::Texture3D ) )
		{
			srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
			srv.Texture3D = {
				.MostDetailedMip = 0,
				.MipLevels = desc.m_mipLevels,
				.ResourceMinLODClamp = 0
			};
		}
		else if ( HasAnyFlags( desc.m_miscFlag, ResourceMisc::TextureCube ) )
		{
			assert( ( desc.m_depth % 6 == 0 ) && "texture cube's depth count must be multiples of 6" );
			if ( desc.m_depth == 6 )
			{
				srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
				srv.TextureCube = {
					.MostDetailedMip = 0,
					.MipLevels = desc.m_mipLevels,
					.ResourceMinLODClamp = 0
				};
			}
			else
			{
				srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
				srv.TextureCubeArray = {
					.MostDetailedMip = 0,
					.MipLevels = desc.m_mipLevels,
					.First2DArrayFace = 0,
					.NumCubes = desc.m_depth / 6,
					.ResourceMinLODClamp = 0
				};
			}
		}
		else
		{
			if ( desc.m_depth <= 1 )
			{
				srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				srv.Texture2D = {
					.MostDetailedMip = 0,
					.MipLevels = desc.m_mipLevels,
					.PlaneSlice = 0,
					.ResourceMinLODClamp = 0
				};
			}
			else
			{
				srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
				srv.Texture2DArray = {
					.MostDetailedMip = 0,
					.MipLevels = desc.m_mipLevels,
					.FirstArraySlice = 0,
					.ArraySize = desc.m_depth,
					.PlaneSlice = 0,
					.ResourceMinLODClamp = 0
				};
			}
		}

		return srv;
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC ConvertToMultiSampleSRVDesc( const TextureDesc& desc )
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srv = {};
		srv.Format = ConvertDxgiFormatForSRV( ConvertFormatToDxgiFormat( desc.m_format ) );
		srv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		assert( ( HasAnyFlags( desc.m_miscFlag, ResourceMisc::Texture3D ) == false )
			&& "texture 3d can't be multi sampled" );
		assert( ( desc.m_height > 1 )
			&& "texture 1d can't be multi sampled" );

		if ( desc.m_depth <= 1 )
		{
			srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
			srv.Texture2DMS = {};
		}
		else
		{
			srv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
			srv.Texture2DMSArray = {
				.FirstArraySlice = 0,
				.ArraySize = desc.m_depth
			};
		}

		return srv;
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC ConvertToSRVDesc( const TextureDesc& desc )
	{
		if ( desc.m_sampleCount <= 1 )
		{
			return ConvertToNonMultiSampleSRVDesc( desc );
		}
		else
		{
			return ConvertToMultiSampleSRVDesc( desc );
		}
	}

	D3D12_UNORDERED_ACCESS_VIEW_DESC ConvertToUAVDesc( const TextureDesc& desc, uint32 mipSlice )
	{
		D3D12_UNORDERED_ACCESS_VIEW_DESC uav = {
			.Format = ConvertFormatToDxgiFormat( desc.m_format )
		};

		if ( HasAnyFlags( desc.m_miscFlag, ResourceMisc::Texture3D ) )
		{
			uav.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
			uav.Texture3D = {
				.MipSlice = mipSlice,
				.FirstWSlice = 0,
				.WSize = desc.m_depth
			};
		}
		else
		{
			if ( desc.m_depth > 1 )
			{
				uav.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
				uav.Texture2DArray = {
					.MipSlice = mipSlice,
					.FirstArraySlice = 0,
					.ArraySize = desc.m_depth,
					.PlaneSlice = 0
				};
			}
			else
			{
				uav.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
				uav.Texture2D = {
					.MipSlice = mipSlice,
					.PlaneSlice = 0
				};
			}
		}

		return uav;
	}

	D3D12_RENDER_TARGET_VIEW_DESC ConvertToNonMultiSampleRTVDesc( const TextureDesc& desc )
	{
		D3D12_RENDER_TARGET_VIEW_DESC rtv = {
			.Format = ConvertFormatToDxgiFormat( desc.m_format )
		};

		if ( HasAnyFlags( desc.m_miscFlag, ResourceMisc::Texture3D ) )
		{
			rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
			rtv.Texture3D = {
				.MipSlice = 0,
				.FirstWSlice = 0,
				.WSize = desc.m_depth
			};
		}
		else
		{
			if ( desc.m_depth > 1 )
			{
				rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
				rtv.Texture2DArray = {
					.MipSlice = 0,
					.FirstArraySlice = 0,
					.ArraySize = desc.m_depth,
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

		return rtv;
	}

	D3D12_RENDER_TARGET_VIEW_DESC ConvertToMultiSampleRTVDesc( const TextureDesc& desc )
	{
		D3D12_RENDER_TARGET_VIEW_DESC rtv = {
			.Format = ConvertFormatToDxgiFormat( desc.m_format )
		};

		assert( ( HasAnyFlags( desc.m_miscFlag, ResourceMisc::Texture3D ) == false )
			&& "texture 3d can't be multi sampled" );
		assert( ( desc.m_height > 1 )
			&& "texture 1d can't be multi sampled" );

		if ( desc.m_depth > 1 )
		{
			rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMSARRAY;
			rtv.Texture2DMSArray = {
				.FirstArraySlice = 0,
				.ArraySize = desc.m_depth
			};
		}
		else
		{
			rtv.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
			rtv.Texture2DMS = {};
		}

		return rtv;
	}

	D3D12_RENDER_TARGET_VIEW_DESC ConvertToRTVDesc( const TextureDesc& desc )
	{
		if ( desc.m_sampleCount > 1 )
		{
			return ConvertToMultiSampleRTVDesc( desc );
		}
		else
		{
			return ConvertToNonMultiSampleRTVDesc( desc );
		}
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC ConvertToNonMultiSampleDSVDesc( const TextureDesc& desc )
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {
			.Format = ConvertDxgiFormatForDSV( ConvertFormatToDxgiFormat( desc.m_format ) )
		};

		assert( HasAnyFlags( desc.m_miscFlag, ResourceMisc::Texture3D ) == false );
		if ( desc.m_depth > 1 )
		{
			dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
			dsv.Texture2DArray = {
				.MipSlice = 0,
				.FirstArraySlice = 0,
				.ArraySize = desc.m_depth
			};
		}
		else
		{
			dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsv.Texture2D = {
				.MipSlice = 0
			};
		}

		return dsv;
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC ConvertToMultiSampleDSVDesc( const TextureDesc& desc )
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {
			.Format = ConvertDxgiFormatForDSV( ConvertFormatToDxgiFormat( desc.m_format ) )
		};

		assert( ( HasAnyFlags( desc.m_miscFlag, ResourceMisc::Texture3D ) == false )
			&& "texture 3d can't be multi sampled" );
		assert( ( desc.m_height > 1 )
			&& "texture 1d can't be multi sampled" );

		if ( desc.m_depth > 1 )
		{
			dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMSARRAY;
			dsv.Texture2DMSArray = {
				.FirstArraySlice = 0,
				.ArraySize = desc.m_depth
			};
		}
		else
		{
			dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
			dsv.Texture2DMS = {};
		}

		return dsv;
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC ConvertToDSVDesc( const TextureDesc& desc )
	{
		if ( desc.m_sampleCount > 1 )
		{
			return ConvertToMultiSampleDSVDesc( desc );
		}
		else
		{
			return ConvertToNonMultiSampleDSVDesc( desc );
		}
	}

	D3D12_HEAP_FLAGS ConvertToTextureHeapFlags( const ResourceBindType bindType )
	{
		if ( HasAnyFlags( bindType, ResourceBindType::RenderTarget | ResourceBindType::DepthStencil ) )
		{
			return D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES;
		}
		else
		{
			return D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;
		}
	}

	D3D12HeapProperties ConvertToHeapProperties( const TextureDesc& desc )
	{
		uint64 alignment = ( desc.m_sampleCount > 1 ) ? D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT : D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;

		bool bDownload = HasAnyFlags( desc.m_access, ResourceAccess::CpuRead );

		D3D12HeapProperties properties = {
			.m_alignment = alignment,
			.m_heapType = ConvertToHeapType( desc.m_access ),
			.m_heapFlags = bDownload ? D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS : ConvertToTextureHeapFlags( desc.m_bindType )
		};

		return properties;
	}

	D3D12_CLEAR_VALUE ConvertToClearValue( const TextureDesc& desc )
	{
		if ( desc.m_clearValue.has_value() == false )
		{
			return {};
		}

		const ResourceClearValue& clearValue = desc.m_clearValue.value();

		D3D12_CLEAR_VALUE ret;
		ResourceFormat clearFormat = ( clearValue.m_format == ResourceFormat::Unknown ) ? desc.m_format : clearValue.m_format;
		ret.Format = IsTypeless( clearFormat ) ? DXGI_FORMAT_UNKNOWN : ConvertFormatToDxgiFormat( clearFormat );

		if ( HasAnyFlags( desc.m_bindType, ResourceBindType::RenderTarget ) )
		{
			ret.Format = ConvertDxgiFormatForDSV( ret.Format );
			ret.Color[0] = clearValue.m_color[0];
			ret.Color[1] = clearValue.m_color[1];
			ret.Color[2] = clearValue.m_color[2];
			ret.Color[3] = clearValue.m_color[3];

			return ret;
		}
		else if ( HasAnyFlags( desc.m_bindType, ResourceBindType::DepthStencil ) )
		{
			ret.Format = ConvertDxgiFormatForDSV( ret.Format );
			ret.DepthStencil.Depth = clearValue.m_depthStencil.m_depth;
			ret.DepthStencil.Stencil = clearValue.m_depthStencil.m_stencil;

			return ret;
		}

		return {};
	}
}

namespace agl
{
	ID3D12Resource* D3D12Texture::Resource()
	{
		return m_resourceInfo.GetResource();
	}

	void* D3D12Texture::Resource() const
	{
		return m_resourceInfo.GetResource();
	}

	const AllocatedResourceInfo& D3D12Texture::GetResourceInfo() const
	{
		return m_resourceInfo;
	}

	const D3D12_RESOURCE_DESC& D3D12Texture::GetD3DDesc() const
	{
		return m_d3dDesc;
	}

	LockedResource D3D12Texture::Lock( uint32 subResource )
	{
		assert( HasAnyFlags( GetDesc().m_access, ResourceAccess::CpuRead ) );

		ID3D12Resource* resource = Resource();
		if ( resource == nullptr )
		{
			return {};
		}

		void* mappedData = nullptr;
		[[maybe_unused]] HRESULT hr = resource->Map( subResource, nullptr, &mappedData );
		assert( SUCCEEDED( hr ) );

		D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout = {};
		uint32 numRows = 0;
		uint64 rowSize = 0;
		uint64 totalSize = 0;

		D3D12Device().GetCopyableFootprints( &GetD3DDesc(), subResource, 1, 0, &layout, &numRows, &rowSize, &totalSize );

		LockedResource result = {
			.m_data = mappedData,
			.m_rowPitch = layout.Footprint.RowPitch,
			.m_depthPitch = layout.Footprint.RowPitch * layout.Footprint.Height
		};

		return result;
	}

	void D3D12Texture::UnLock( uint32 subResource )
	{
		if ( Resource() == nullptr )
		{
			return;
		}

		Resource()->Unmap( subResource, nullptr );
	}

	void D3D12Texture::CreateShaderResource( std::optional<ResourceFormat> overrideFormat )
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = ConvertToSRVDesc( m_desc );
		if ( overrideFormat.has_value() )
		{
			srvDesc.Format = ConvertFormatToDxgiFormat( *overrideFormat );
		}
		m_srv = new D3D12ShaderResourceView( this, Resource(), srvDesc );
		m_srv->Init();
	}

	void D3D12Texture::CreateUnorderedAccess( std::optional<ResourceFormat> overrideFormat )
	{
		m_uav.resize( m_desc.m_mipLevels );
		for ( uint32 mipSlice = 0; mipSlice < m_desc.m_mipLevels; ++mipSlice )
		{
			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = ConvertToUAVDesc( m_desc, mipSlice );
			if ( overrideFormat.has_value() )
			{
				uavDesc.Format = ConvertFormatToDxgiFormat( *overrideFormat );
			}
			m_uav[mipSlice] = new D3D12UnorderedAccessView( this, Resource(), uavDesc );
			m_uav[mipSlice]->Init();
		}
	}

	void D3D12Texture::UpdateTextureMips( uint32 width, uint32 height, int32 mipLevels, const ResourceInitData& initData )
	{
		TextureBase::UpdateTextureMips( width, height, mipLevels, initData );

		m_d3dDesc = ConvertToD3DDesc( m_desc );

		SetInitData( initData );

		// Keep the resource alive because it may still be referenced by GPU commands
		D3D12FrameResources().RegisterResource( this );

		CreateTexture();

		if ( m_srv.Get() )
		{
			static_cast<D3D12ShaderResourceView*>( m_srv.Get() )->UpdateTextureMips( Resource(), m_desc.m_mipLevels );
		}
	}

	void D3D12Texture::Reconstruct( const TextureDesc& desc, const ResourceInitData* initData )
	{
		delete[] m_dataStorage;

		m_desc = desc;
		m_d3dDesc = ConvertToD3DDesc( desc );
		m_initData.clear();

		if ( initData )
		{
			m_dataStorage = new unsigned char[initData->m_srcSize];
			std::memcpy( m_dataStorage, initData->m_srcData, initData->m_srcSize );

			SetInitData( *initData );
		}
	}

	D3D12Texture::D3D12Texture( const TextureDesc& desc, const char* debugName, ResourceState initialState, const ResourceInitData* initData )
		: TextureBase( desc, debugName, initialState, initData )
		, m_d3dDesc( ConvertToD3DDesc( desc ) )
	{
		if ( initData )
		{
			SetInitData( *initData );
		}
	}

	void D3D12Texture::SetDebugObjectName()
	{
		if ( ID3D12Resource* resource = m_resourceInfo.GetResource() )
		{
			auto dataSize = static_cast<uint32>( m_debugName.Str().size() );
			resource->SetPrivateData( WKPDID_D3DDebugObjectName, dataSize, m_debugName.Str().data() );
		}
	}

	void D3D12Texture::CreateTexture()
	{
		AdjustInitalResourceStates();

		D3D12HeapProperties properties = ConvertToHeapProperties( m_desc );

		D3D12_CLEAR_VALUE clearValue = ConvertToClearValue( m_desc );

		bool bDownload = HasAnyFlags( m_desc.m_access, ResourceAccess::CpuRead );
		m_resourceInfo = D3D12Allocator().AllocateResource(
			properties,
			bDownload ? GetDescForDownload() : m_d3dDesc,
			ConvertToResourceStates( GetResourceState() ),
			clearValue.Format == DXGI_FORMAT_UNKNOWN ? nullptr : &clearValue );

		SetDebugObjectName();

		if ( m_initData.empty() == false )
		{
			for ( size_t i = 0; i < m_initData.size(); ++i )
			{
				D3D12Uploader().Upload( *this, m_initData[i].pData, static_cast<uint32>( m_initData[i].RowPitch ), nullptr, static_cast<uint32>( i ) );
			}
		}
	}

	void D3D12Texture::AdjustInitalResourceStates()
	{
		if ( HasAllFlags( m_desc.m_access, ResourceAccess::Readback ) )
		{
			SetResourceState( ResourceState::CopyDest );
		}
		else if ( HasAllFlags( m_desc.m_access, ResourceAccess::Upload ) )
		{
			SetResourceState( ResourceState::GenericRead );
		}
		else if ( ( m_dataStorage != nullptr ) && ( HasAllFlags( m_desc.m_access, ResourceAccess::CpuWrite ) == false ) )
		{
			SetResourceState( ResourceState::Common );
		}
	}

	void D3D12Texture::SetInitData( const ResourceInitData& initData )
	{
		size_t numSections = initData.m_sections.size();

		m_initData.resize( numSections );
		for ( size_t i = 0; i < numSections; ++i )
		{
			const ResourceSectionData& section = initData.m_sections[i];

			m_initData[i].pData = m_dataStorage + section.m_offset;
			m_initData[i].RowPitch = section.m_pitch;
			m_initData[i].SlicePitch = section.m_slicePitch;
		}
	}

	D3D12_RESOURCE_DESC D3D12Texture::GetDescForDownload() const
	{
		D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout = {};
		uint32 numRows = 0;
		uint64 rowSize = 0;
		uint64 totalSize = 0;

		D3D12Device().GetCopyableFootprints( &m_d3dDesc, 0, 1, 0, &layout, &numRows, &rowSize, &totalSize );

		return {
			.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
			.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT,
			.Width = totalSize,
			.Height = 1,
			.DepthOrArraySize = 1,
			.MipLevels = 1,
			.Format = DXGI_FORMAT_UNKNOWN,
			.SampleDesc = {
				.Count = 1,
				.Quality = 0
			},
			.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
			.Flags = GetResourceFlags( m_desc )
		};
	}

	D3D12Texture2D::D3D12Texture2D( const TextureDesc& desc, const char* debugName, ResourceState initialState, const ResourceInitData* initData )
		: D3D12Texture( desc, debugName, initialState, initData ) {}

	D3D12Texture2D::D3D12Texture2D( ID3D12Resource* texture, const char* debugName, const float4& clearColor, const D3D12_RESOURCE_DESC* desc )
	{
		if ( texture )
		{
			m_resourceInfo.SetResource( texture );

			Rename( Name( debugName ) );

			if ( desc == nullptr )
			{
				m_d3dDesc = texture->GetDesc();
			}
			else
			{
				m_d3dDesc = *desc;
			}

			[[maybe_unused]] HRESULT hr = texture->GetHeapProperties( &m_heapProperties, &m_heapFlags );
			assert( SUCCEEDED( hr ) );

			m_desc = ConvertToTextureDesc( m_d3dDesc, m_heapProperties );
			m_desc.m_clearValue = ResourceClearValue{
				.m_format = m_desc.m_format,
				.m_color = { clearColor[0], clearColor[1], clearColor[2], clearColor[3]}
			};
		}
	}

	void D3D12Texture2D::CreateRenderTarget( std::optional<ResourceFormat> overrideFormat )
	{
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = ConvertToRTVDesc( m_desc );
		if ( overrideFormat.has_value() )
		{
			rtvDesc.Format = ConvertFormatToDxgiFormat( *overrideFormat );
		}

		ColorF clearColor = ColorF::Black;
		if ( m_desc.m_clearValue )
		{
			clearColor = m_desc.m_clearValue->m_color;
		}

		m_rtv = new D3D12RenderTargetView( this, static_cast<ID3D12Resource*>( Resource() ), rtvDesc, clearColor );
		m_rtv->Init();
	}

	void D3D12Texture2D::CreateDepthStencil( std::optional<ResourceFormat> overrideFormat )
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = ConvertToDSVDesc( m_desc );
		if ( overrideFormat.has_value() )
		{
			dsvDesc.Format = ConvertFormatToDxgiFormat( *overrideFormat );
		}

		float depthClearValue = 0;
		uint8 stencilClearValue = 0;
		if ( m_desc.m_clearValue )
		{
			depthClearValue = m_desc.m_clearValue->m_depthStencil.m_depth;
			stencilClearValue = m_desc.m_clearValue->m_depthStencil.m_stencil;
		}

		m_dsv = new D3D12DepthStencilView( this, static_cast<ID3D12Resource*>( Resource() ), dsvDesc, depthClearValue, stencilClearValue );
		m_dsv->Init();
	}

	void D3D12Texture3D::CreateRenderTarget( std::optional<ResourceFormat> overrideFormat )
	{
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = ConvertToRTVDesc( m_desc );
		if ( overrideFormat.has_value() )
		{
			rtvDesc.Format = ConvertFormatToDxgiFormat( *overrideFormat );
		}

		ColorF clearColor = ColorF::Black;
		if ( m_desc.m_clearValue )
		{
			clearColor = m_desc.m_clearValue->m_color;
		}

		m_rtv = new D3D12RenderTargetView( this, static_cast<ID3D12Resource*>( Resource() ), rtvDesc, clearColor );
		m_rtv->Init();
	}

	D3D12Texture3D::D3D12Texture3D( const TextureDesc& desc, const char* debugName, ResourceState initialState, const ResourceInitData* initData )
		: D3D12Texture( desc, debugName, initialState, initData )
	{
	}
}
