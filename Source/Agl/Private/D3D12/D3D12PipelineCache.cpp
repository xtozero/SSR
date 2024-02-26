#include "D3D12PipelineCache.h"

#include "D3D12Api.h"
#include "D3D12BindlessManager.h"
#include "D3D12Buffer.h"
#include "D3D12FlagConvertor.h"
#include "D3D12GlobalDescriptorHeap.h"
#include "D3D12NullDescriptor.h"
#include "D3D12PipelineState.h"
#include "D3D12ResourceManager.h"
#include "D3D12SamplerState.h"
#include "D3D12Texture.h"

#include "GlobalConstantBuffers.h"

#include "InlineMemoryAllocator.h"

#include "ShaderBindings.h"

#include <vector>

namespace agl
{
	void D3D12PipelineCache::Prepare()
	{
		std::memset( m_vertexBufferViews, 0, sizeof( m_vertexBufferViews ) );
		m_numVertexBufferViews = 0;

		std::memset( &m_indexBufferView, 0, sizeof( m_indexBufferView ) );

		m_pipelineState = nullptr;

		std::memset( m_viewports, 0, sizeof( m_viewports ) );
		m_numViewports = 0;

		std::memset( m_siccorRects, 0, sizeof( m_siccorRects ) );
		m_numSiccorRects = 0;

		std::memset( m_rtvs, 0, sizeof( m_rtvs ) );
		m_dsv = nullptr;
	}

	void D3D12PipelineCache::ReleaseRenderResources()
	{
		for ( auto& allocatedInfo : m_allocatedInfos )
		{
			allocatedInfo.Release();
		}

		m_allocatedInfos.clear();
		m_allocatedIdentifiers.clear();

		m_allocatedIdentifierAllocator.Flush();
		m_allocatedInfoAllocator.Flush();

		std::construct_at( &m_allocatedIdentifiers, m_allocatedIdentifierAllocator );
		std::construct_at( &m_allocatedInfos, m_allocatedInfoAllocator );

		m_residentResource.clear();
	}

	void D3D12PipelineCache::BindVertexBuffer( ID3D12GraphicsCommandList6& commandList, Buffer* const* vertexBuffers, uint32 startSlot, uint32 numBuffers, const uint32* pOffsets )
	{
		std::vector<D3D12_VERTEX_BUFFER_VIEW, InlineAllocator<D3D12_VERTEX_BUFFER_VIEW, 3>> vertexBufferViews;
		vertexBufferViews.resize( numBuffers, {} );

		if ( vertexBuffers )
		{
			for ( uint32 i = 0; i < numBuffers; ++i )
			{
				auto vertexBuffer = static_cast<D3D12VertexBuffer*>( vertexBuffers[i] );
				if ( vertexBuffer )
				{
					D3D12_VERTEX_BUFFER_VIEW& view = vertexBufferViews[i];

					view = vertexBuffer->GetView();
					view.BufferLocation += pOffsets[i];
					view.SizeInBytes -= pOffsets[i];
				}
			}
		}

		if ( m_numVertexBufferViews == numBuffers )
		{
			auto pred = []( const D3D12_VERTEX_BUFFER_VIEW& lhs, const D3D12_VERTEX_BUFFER_VIEW& rhs )
			{
				return lhs.BufferLocation == rhs.BufferLocation
					&& lhs.SizeInBytes == rhs.SizeInBytes
					&& lhs.StrideInBytes == rhs.StrideInBytes;
			};

			bool alreadyBound = std::equal( std::begin( vertexBufferViews ), std::end( vertexBufferViews ), std::begin( m_vertexBufferViews ), pred );

			if ( alreadyBound )
			{
				return;
			}
		}

		if ( vertexBuffers != nullptr )
		{
			for ( uint32 i = 0; i < numBuffers; ++i )
			{
				RegisterRenderResource( vertexBuffers[i] );
			}
		}

		std::copy( std::begin( vertexBufferViews ), std::end( vertexBufferViews ), std::begin( m_vertexBufferViews ) );
		m_numVertexBufferViews = numBuffers;

		commandList.IASetVertexBuffers( startSlot, numBuffers, vertexBufferViews.data() );
	}

	void D3D12PipelineCache::BindIndexBuffer( ID3D12GraphicsCommandList6& commandList, Buffer* indexBuffer, uint32 indexOffset )
	{
		D3D12_INDEX_BUFFER_VIEW view = {};

		if ( auto d3d12Buffer = static_cast<D3D12IndexBuffer*>( indexBuffer ) )
		{
			view = d3d12Buffer->GetView();
			view.BufferLocation += indexOffset;
			view.SizeInBytes -= indexOffset;
		}

		bool alreadyBound = ( std::memcmp( &m_indexBufferView, &view, sizeof( D3D12_INDEX_BUFFER_VIEW ) ) == 0 );
		if ( alreadyBound )
		{
			return;
		}

		RegisterRenderResource( indexBuffer );

		m_indexBufferView = view;
		commandList.IASetIndexBuffer( ( view.SizeInBytes == 0 ) ? nullptr : &view );
	}

	void D3D12PipelineCache::BindPipelineState( ID3D12GraphicsCommandList6& commandList, GraphicsPipelineState* pipelineState )
	{
		if ( pipelineState == nullptr )
		{
			return;
		}

		auto d3d12GraphicsPipelineState = static_cast<D3D12GraphicsPipelineState*>( pipelineState );
		assert( d3d12GraphicsPipelineState != nullptr );

		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = d3d12GraphicsPipelineState->GetDesc();
		desc.NumRenderTargets = D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT;
		for ( int32 i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i )
		{
			desc.RTVFormats[i] = m_rtvs[i] ? ( static_cast<D3D12RenderTargetView*>( m_rtvs[i] )->GetDesc().Format ) : DXGI_FORMAT_UNKNOWN;
		}
		desc.DSVFormat = m_dsv ? ( static_cast<D3D12DepthStencilView*>( m_dsv )->GetDesc().Format ) : DXGI_FORMAT_UNKNOWN;

		auto& d3d12ResourceManager = *static_cast<D3D12ResourceManager*>( GetInterface<IResourceManager>() );
		ID3D12PipelineState* graphicsPipelineState = d3d12ResourceManager.FindOrCreate( pipelineState, desc );

		if ( m_pipelineState == graphicsPipelineState )
		{
			return;
		}

		ID3D12RootSignature* rootSignature = nullptr;
		if ( D3D12RootSignature* d3d12RootSignature = d3d12GraphicsPipelineState->GetRootSignature() )
		{
			rootSignature = d3d12RootSignature->Resource();
		}

		RegisterRenderResource( pipelineState );

		m_pipelineState = graphicsPipelineState;
		commandList.IASetPrimitiveTopology( d3d12GraphicsPipelineState->GetPrimitiveTopology() );
		commandList.SetPipelineState( graphicsPipelineState );
		commandList.SetGraphicsRootSignature( rootSignature );
	}

	void D3D12PipelineCache::BindPipelineState( ID3D12GraphicsCommandList6& commandList, ComputePipelineState* pipelineState )
	{
		if ( pipelineState == nullptr )
		{
			return;
		}

		auto d3d12ComputePipelineState = static_cast<D3D12ComputePipelineState*>( pipelineState );
		assert( d3d12ComputePipelineState != nullptr );

		ID3D12PipelineState* computePipelineState = d3d12ComputePipelineState->Resource();

		if ( m_pipelineState == computePipelineState )
		{
			return;
		}

		ID3D12RootSignature* rootSignature = nullptr;
		if ( D3D12RootSignature* d3d12RootSignature = d3d12ComputePipelineState->GetRootSignature() )
		{
			rootSignature = d3d12RootSignature->Resource();
		}

		RegisterRenderResource( pipelineState );

		m_pipelineState = computePipelineState;
		commandList.SetPipelineState( computePipelineState );
		commandList.SetComputeRootSignature( rootSignature );
	}

	void D3D12PipelineCache::BindBindlessResources( ID3D12GraphicsCommandList6& commandList, GlobalConstantBuffers& globalConstantBuffers, ShaderBindings& shaderBindings )
	{
		for ( uint32 shaderType = 0; shaderType < MAX_SHADER_TYPE<uint32>; ++shaderType )
		{
			SingleShaderBindings binding = shaderBindings.GetSingleShaderBindings( static_cast<ShaderType>( shaderType ) );
			if ( binding.GetShaderType() == ShaderType::None )
			{
				continue;
			}

			const ShaderParameterInfo& parameterInfo = binding.ParameterInfo();

			for ( uint32 i = 0; i < binding.NumBindless(); ++i )
			{
				RefHandle<GraphicsApiResource> resource = binding.GetBindlessStart()[i];
				int32 bindlessHandle = ( resource != nullptr ) ? resource->GetBindlessHandle() : -1;

				const ShaderParameter& bindlessParam = parameterInfo.m_bindless[i];
				globalConstantBuffers.SetShaderValue( bindlessParam, &bindlessHandle );
			}
		}

		globalConstantBuffers.AddGlobalConstantBuffers( shaderBindings );

		ID3D12DescriptorHeap* heaps[] = {
			D3D12BindlessMgr().GetHeap().Resource(),
			D3D12BindlessMgr().GetSamplerHeap().Resource()
		};

		RegisterRenderResource( heaps[0] );
		RegisterRenderResource( heaps[1] );

		commandList.SetDescriptorHeaps( std::extent_v<decltype( heaps )>, heaps );

		auto GetBindlessGpuHandle = []( D3D12DescriptorHeap& heap, int32 bindlessHandle ) -> D3D12_GPU_DESCRIPTOR_HANDLE
			{
				return heap.GetGpuHandle().At( bindlessHandle );
			};

		uint32 rootParameterIndex = 2;

		if ( shaderBindings.IsCompute() )
		{
			commandList.SetComputeRootDescriptorTable( 0, D3D12BindlessMgr().GetHeap().GetGpuHandle().At() );
			commandList.SetComputeRootDescriptorTable( 1, D3D12BindlessMgr().GetSamplerHeap().GetGpuHandle().At() );

			for ( uint32 shaderType = 0; shaderType < MAX_SHADER_TYPE<uint32>; ++shaderType )
			{
				SingleShaderBindings binding = shaderBindings.GetSingleShaderBindings( static_cast<ShaderType>( shaderType ) );
				if ( binding.GetShaderType() == ShaderType::None )
				{
					continue;
				}

				const ShaderParameterInfo& parameterInfo = binding.ParameterInfo();

				for ( uint32 i = 0; i < binding.NumSRV(); ++i )
				{
					if ( parameterInfo.m_srvs[i].m_space >= 100 )
					{
						continue;
					}

					RefHandle<ShaderResourceView> srv = binding.GetSRVStart()[i];

					auto d3d12SRV = static_cast<D3D12ShaderResourceView*>( srv.Get() );
					assert( d3d12SRV != nullptr );

					RegisterRenderResource( d3d12SRV->GetOwner() );

					int32 bindlessHandle = d3d12SRV->GetBindlessHandle();
					commandList.SetComputeRootDescriptorTable( rootParameterIndex++, GetBindlessGpuHandle( D3D12BindlessMgr().GetHeap(), bindlessHandle ) );
				}

				for ( uint32 i = 0; i < binding.NumUAV(); ++i )
				{
					RefHandle<UnorderedAccessView> uav = binding.GetUAVStart()[i];

					auto d3d12UAV = static_cast<D3D12UnorderedAccessView*>( uav.Get() );
					assert( d3d12UAV != nullptr );

					RegisterRenderResource( d3d12UAV->GetOwner() );

					int32 bindlessHandle = d3d12UAV->GetBindlessHandle();
					commandList.SetComputeRootDescriptorTable( rootParameterIndex++, GetBindlessGpuHandle( D3D12BindlessMgr().GetHeap(), bindlessHandle ) );
				}

				for ( uint32 i = 0; i < binding.NumCBV(); ++i )
				{
					RefHandle<Buffer> cb = binding.GetConstantBufferStart()[i];

					auto d3d12CB = static_cast<D3D12Buffer*>( cb.Get() );
					assert( d3d12CB != nullptr );

					RegisterRenderResource( cb.Get() );

					int32 bindlessHandle = d3d12CB->CBV()->GetBindlessHandle();
					commandList.SetComputeRootDescriptorTable( rootParameterIndex++, GetBindlessGpuHandle( D3D12BindlessMgr().GetHeap(), bindlessHandle ) );
				}

				for ( uint32 i = 0; i < binding.NumSampler(); ++i )
				{
					if ( parameterInfo.m_samplers[i].m_space == 100 )
					{
						continue;
					}

					RefHandle<SamplerState> samplerState = binding.GetSamplerStart()[i];

					auto d3d12SamplerState = static_cast<D3D12SamplerState*>( samplerState.Get() );
					assert( d3d12SamplerState != nullptr );

					int32 bindlessHandle = d3d12SamplerState->GetBindlessHandle();
					commandList.SetComputeRootDescriptorTable( rootParameterIndex++, GetBindlessGpuHandle( D3D12BindlessMgr().GetSamplerHeap(), bindlessHandle ) );
				}
			}
		}
		else
		{
			commandList.SetGraphicsRootDescriptorTable( 0, D3D12BindlessMgr().GetHeap().GetGpuHandle().At() );
			commandList.SetGraphicsRootDescriptorTable( 1, D3D12BindlessMgr().GetSamplerHeap().GetGpuHandle().At() );

			for ( uint32 shaderType = 0; shaderType < MAX_SHADER_TYPE<uint32>; ++shaderType )
			{
				SingleShaderBindings binding = shaderBindings.GetSingleShaderBindings( static_cast<ShaderType>( shaderType ) );
				if ( binding.GetShaderType() == ShaderType::None )
				{
					continue;
				}

				const ShaderParameterInfo& parameterInfo = binding.ParameterInfo();

				for ( uint32 i = 0; i < binding.NumSRV(); ++i )
				{
					if ( parameterInfo.m_srvs[i].m_space >= 100 )
					{
						continue;
					}

					RefHandle<ShaderResourceView> srv = binding.GetSRVStart()[i];

					auto d3d12SRV = static_cast<D3D12ShaderResourceView*>( srv.Get() );
					assert( d3d12SRV != nullptr );

					RegisterRenderResource( d3d12SRV->GetOwner() );

					int32 bindlessHandle = d3d12SRV->GetBindlessHandle();
					commandList.SetGraphicsRootDescriptorTable( rootParameterIndex++, GetBindlessGpuHandle( D3D12BindlessMgr().GetHeap(), bindlessHandle ) );
				}

				for ( uint32 i = 0; i < binding.NumUAV(); ++i )
				{
					RefHandle<UnorderedAccessView> uav = binding.GetUAVStart()[i];

					auto d3d12UAV = static_cast<D3D12UnorderedAccessView*>( uav.Get() );
					assert( d3d12UAV != nullptr );

					RegisterRenderResource( d3d12UAV->GetOwner() );

					int32 bindlessHandle = d3d12UAV->GetBindlessHandle();
					commandList.SetGraphicsRootDescriptorTable( rootParameterIndex++, GetBindlessGpuHandle( D3D12BindlessMgr().GetHeap(), bindlessHandle ) );
				}

				for ( uint32 i = 0; i < binding.NumCBV(); ++i )
				{
					RefHandle<Buffer> cb = binding.GetConstantBufferStart()[i];

					auto d3d12CB = static_cast<D3D12Buffer*>( cb.Get() );
					assert( d3d12CB != nullptr );

					RegisterRenderResource( cb.Get() );

					int32 bindlessHandle = d3d12CB->CBV()->GetBindlessHandle();
					commandList.SetGraphicsRootDescriptorTable( rootParameterIndex++, GetBindlessGpuHandle( D3D12BindlessMgr().GetHeap(), bindlessHandle ) );
				}

				for ( uint32 i = 0; i < binding.NumSampler(); ++i )
				{
					if ( parameterInfo.m_samplers[i].m_space == 100 )
					{
						continue;
					}

					RefHandle<SamplerState> samplerState = binding.GetSamplerStart()[i];

					auto d3d12SamplerState = static_cast<D3D12SamplerState*>( samplerState.Get() );
					assert( d3d12SamplerState != nullptr );

					int32 bindlessHandle = d3d12SamplerState->GetBindlessHandle();
					commandList.SetGraphicsRootDescriptorTable( rootParameterIndex++, GetBindlessGpuHandle( D3D12BindlessMgr().GetSamplerHeap(), bindlessHandle ) );
				}
			}
		}
	}

	void D3D12PipelineCache::BindShaderResources( ID3D12GraphicsCommandList6& commandList, D3D12GlobalDescriptorHeap& descriptorHeap, GlobalConstantBuffers& globalConstantBuffers, ShaderBindings& shaderBindings )
	{
		globalConstantBuffers.AddGlobalConstantBuffers( shaderBindings );

		uint32 numSrvUavCbv = 0;
		uint32 numSampler = 0;

		for ( uint32 shaderType = 0; shaderType < MAX_SHADER_TYPE<uint32>; ++shaderType )
		{
			SingleShaderBindings binding = shaderBindings.GetSingleShaderBindings( static_cast<ShaderType>( shaderType ) );
			if ( binding.GetShaderType() == ShaderType::None )
			{
				continue;
			}

			numSrvUavCbv += binding.NumSRV() + binding.NumUAV() + binding.NumCBV();
			numSampler += binding.NumSampler();
		}

		D3D12GlobalHeapAllocatedInfo allocatedInfo[2] = {
			descriptorHeap.Aquire( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, numSrvUavCbv ),
			descriptorHeap.Aquire( D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, numSampler )
		};

		constexpr uint32 SrvCbvUavHeap = 0;
		constexpr uint32 SamplerHeap = 1;

		uint32 destOffset = 0;
		for ( uint32 shaderType = 0; shaderType < MAX_SHADER_TYPE<uint32>; ++shaderType )
		{
			SingleShaderBindings binding = shaderBindings.GetSingleShaderBindings( static_cast<ShaderType>( shaderType ) );
			if ( binding.GetShaderType() == ShaderType::None )
			{
				continue;
			}

			for ( uint32 i = 0; i < binding.NumSRV(); ++i )
			{
				RefHandle<ShaderResourceView> srv = binding.GetSRVStart()[i];

				auto d3d12SRV = static_cast<D3D12ShaderResourceView*>( srv.Get() );
				assert( d3d12SRV != nullptr );

				RegisterRenderResource( d3d12SRV->GetOwner() );
				
				D3D12Device().CopyDescriptorsSimple( 1, allocatedInfo[SrvCbvUavHeap].GetCpuHandle( destOffset ), d3d12SRV->GetCpuHandle().At(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
				++destOffset;
			}
		}

		for ( uint32 shaderType = 0; shaderType < MAX_SHADER_TYPE<uint32>; ++shaderType )
		{
			SingleShaderBindings binding = shaderBindings.GetSingleShaderBindings( static_cast<ShaderType>( shaderType ) );
			if ( binding.GetShaderType() == ShaderType::None )
			{
				continue;
			}

			for ( uint32 i = 0; i < binding.NumUAV(); ++i )
			{
				RefHandle<UnorderedAccessView>* uav = binding.GetUAVStart() + i;

				auto d3d12UAV = static_cast<D3D12UnorderedAccessView*>( uav->Get() );
				assert( d3d12UAV != nullptr );

				RegisterRenderResource( d3d12UAV->GetOwner() );

				D3D12Device().CopyDescriptorsSimple( 1, allocatedInfo[SrvCbvUavHeap].GetCpuHandle( destOffset ), d3d12UAV->GetCpuHandle().At(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
				++destOffset;
			}
		}

		for ( uint32 shaderType = 0; shaderType < MAX_SHADER_TYPE<uint32>; ++shaderType )
		{
			SingleShaderBindings binding = shaderBindings.GetSingleShaderBindings( static_cast<ShaderType>( shaderType ) );
			if ( binding.GetShaderType() == ShaderType::None )
			{
				continue;
			}

			for ( uint32 i = 0; i < binding.NumCBV(); ++i )
			{
				RefHandle<Buffer>* cb = binding.GetConstantBufferStart() + i;

				auto d3d12CB = static_cast<D3D12Buffer*>( cb->Get() );
				assert( d3d12CB != nullptr );

				RegisterRenderResource( cb->Get() );

				D3D12Device().CopyDescriptorsSimple( 1, allocatedInfo[SrvCbvUavHeap].GetCpuHandle( destOffset ), d3d12CB->CBV()->GetCpuHandle().At(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV );
				++destOffset;
			}
		}

		for ( uint32 shaderType = 0; shaderType < MAX_SHADER_TYPE<uint32>; ++shaderType )
		{
			SingleShaderBindings binding = shaderBindings.GetSingleShaderBindings( static_cast<ShaderType>( shaderType ) );
			if ( binding.GetShaderType() == ShaderType::None )
			{
				continue;
			}

			for ( uint32 i = 0; i < binding.NumSampler(); ++i )
			{
				RefHandle<SamplerState>* samplerState = binding.GetSamplerStart() + i;

				auto d3d12SamplerState = static_cast<D3D12SamplerState*>( samplerState->Get() );
				assert( d3d12SamplerState != nullptr );

				D3D12Device().CopyDescriptorsSimple( 1, allocatedInfo[SamplerHeap].GetCpuHandle( i ), d3d12SamplerState->Resource().GetCpuHandle().At(), D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER );
			}
		}

		std::vector<ID3D12DescriptorHeap*, InlineAllocator<ID3D12DescriptorHeap*, 2>> descriptorHeaps;
		if ( numSrvUavCbv > 0 )
		{
			descriptorHeaps.push_back( allocatedInfo[SrvCbvUavHeap].GetDescriptorHeap() );
		}

		if ( numSampler > 0 )
		{
			descriptorHeaps.push_back( allocatedInfo[SamplerHeap].GetDescriptorHeap() );
		}

		commandList.SetDescriptorHeaps( static_cast<uint32>( descriptorHeaps.size() ), descriptorHeaps.data() );

		uint32 rootParameterIndex = 0;
		if ( shaderBindings.IsCompute() )
		{
			uint32 offset = 0;
			SingleShaderBindings binding = shaderBindings.GetSingleShaderBindings( ShaderType::CS );

			if ( binding.NumSRV() > 0 )
			{
				commandList.SetComputeRootDescriptorTable( rootParameterIndex++, allocatedInfo[SrvCbvUavHeap].GetGpuHandle( offset ) );
				offset += binding.NumSRV();
			}

			if ( binding.NumUAV() > 0 )
			{
				commandList.SetComputeRootDescriptorTable( rootParameterIndex++, allocatedInfo[SrvCbvUavHeap].GetGpuHandle( offset ) );
				offset += binding.NumUAV();
			}

			if ( binding.NumCBV() > 0 )
			{
				commandList.SetComputeRootDescriptorTable( rootParameterIndex++, allocatedInfo[SrvCbvUavHeap].GetGpuHandle( offset ) );
				offset += binding.NumCBV();
			}

			if ( binding.NumSampler() > 0 )
			{
				commandList.SetComputeRootDescriptorTable( rootParameterIndex++, allocatedInfo[SamplerHeap].GetGpuHandle( 0 ) );
			}
		}
		else
		{
			uint32 offset = 0;
			for ( uint32 shaderType = 0; shaderType < MAX_SHADER_TYPE<uint32>; ++shaderType )
			{
				SingleShaderBindings binding = shaderBindings.GetSingleShaderBindings( static_cast<ShaderType>( shaderType ) );
				if ( binding.GetShaderType() == ShaderType::None )
				{
					continue;
				}

				if ( binding.NumSRV() > 0 )
				{
					commandList.SetGraphicsRootDescriptorTable( rootParameterIndex++, allocatedInfo[SrvCbvUavHeap].GetGpuHandle( offset ) );
					offset += binding.NumSRV();
				}
			}

			for ( uint32 shaderType = 0; shaderType < MAX_SHADER_TYPE<uint32>; ++shaderType )
			{
				SingleShaderBindings binding = shaderBindings.GetSingleShaderBindings( static_cast<ShaderType>( shaderType ) );
				if ( binding.GetShaderType() == ShaderType::None )
				{
					continue;
				}

				if ( binding.NumUAV() > 0 )
				{
					commandList.SetGraphicsRootDescriptorTable( rootParameterIndex++, allocatedInfo[SrvCbvUavHeap].GetGpuHandle( offset ) );
					offset += binding.NumUAV();
				}
			}

			for ( uint32 shaderType = 0; shaderType < MAX_SHADER_TYPE<uint32>; ++shaderType )
			{
				SingleShaderBindings binding = shaderBindings.GetSingleShaderBindings( static_cast<ShaderType>( shaderType ) );
				if ( binding.GetShaderType() == ShaderType::None )
				{
					continue;
				}

				if ( binding.NumCBV() > 0 )
				{
					commandList.SetGraphicsRootDescriptorTable( rootParameterIndex++, allocatedInfo[SrvCbvUavHeap].GetGpuHandle( offset ) );
					offset += binding.NumCBV();
				}
			}

			offset = 0;
			for ( uint32 shaderType = 0; shaderType < MAX_SHADER_TYPE<uint32>; ++shaderType )
			{
				SingleShaderBindings binding = shaderBindings.GetSingleShaderBindings( static_cast<ShaderType>( shaderType ) );
				if ( binding.GetShaderType() == ShaderType::None )
				{
					continue;
				}

				if ( binding.NumSampler() > 0 )
				{
					commandList.SetGraphicsRootDescriptorTable( rootParameterIndex++, allocatedInfo[SamplerHeap].GetGpuHandle( offset ) );
					offset += binding.NumSampler();
				}
			}
		}
	}

	void D3D12PipelineCache::SetViewports( ID3D12GraphicsCommandList6& commandList, uint32 count, const CubeArea<float>* area )
	{
		D3D12_VIEWPORT viewports[D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE] = {};
		static_assert( sizeof( viewports ) == sizeof( m_viewports ) );

		for ( uint32 i = 0; i < count; ++i )
		{
			viewports[i] = {
				.TopLeftX = area[i].m_left,
				.TopLeftY = area[i].m_top,
				.Width = area[i].m_right - area[i].m_left,
				.Height = area[i].m_bottom - area[i].m_top,
				.MinDepth = area[i].m_front,
				.MaxDepth = area[i].m_back
			};
		}

		auto pred = []( const D3D12_VIEWPORT& lhs, const D3D12_VIEWPORT& rhs )
		{
			return lhs.TopLeftX == rhs.TopLeftX
				&& lhs.TopLeftY == rhs.TopLeftY
				&& lhs.Width == rhs.Width
				&& lhs.Height == rhs.Height
				&& lhs.MinDepth == rhs.MinDepth
				&& lhs.MaxDepth == rhs.MaxDepth;
		};

		if ( std::equal( std::begin( viewports ), std::end( viewports ), std::begin( m_viewports ), pred ) )
		{
			return;
		}

		std::copy( std::begin( viewports ), std::end( viewports ), std::begin( m_viewports ) );
		m_numViewports = count;

		commandList.RSSetViewports( count, viewports );
	}

	void D3D12PipelineCache::SetScissorRects( ID3D12GraphicsCommandList6& commandList, uint32 count, const RectangleArea<int32>* area )
	{
		D3D12_RECT rects[D3D12_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE] = {};
		static_assert( sizeof( rects ) == sizeof( m_siccorRects ) );

		for ( uint32 i = 0; i < count; ++i )
		{
			rects[i] = {
				.left = area[i].m_left,
				.top = area[i].m_top,
				.right = area[i].m_right,
				.bottom = area[i].m_bottom
			};
		}

		auto pred = []( const D3D12_RECT& lhs, const D3D12_RECT& rhs )
		{
			return lhs.left == rhs.left
				&& lhs.top == rhs.top
				&& lhs.right == rhs.right
				&& lhs.bottom == rhs.bottom;
		};

		if ( std::equal( std::begin( rects ), std::end( rects ), std::begin( m_siccorRects ), pred ) )
		{
			return;
		}

		std::copy( std::begin( rects ), std::end( rects ), std::begin( m_siccorRects ) );
		m_numSiccorRects = count;

		commandList.RSSetScissorRects( count, rects );
	}

	void D3D12PipelineCache::BindRenderTargets( ID3D12GraphicsCommandList6& commandList, RenderTargetView** pRenderTargets, uint32 renderTargetCount, DepthStencilView* depthStencil )
	{
		assert( pRenderTargets != nullptr );
		if ( m_dsv == depthStencil
			&& std::equal( pRenderTargets, pRenderTargets + renderTargetCount, std::begin( m_rtvs ) ) )
		{
			return;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE rtvs[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};

		for ( uint32 i = 0; i < renderTargetCount; ++i )
		{
			auto d3d12RTV = static_cast<D3D12RenderTargetView*>( pRenderTargets[i] );
			if ( d3d12RTV )
			{
				rtvs[i] = d3d12RTV->GetCpuHandle().At();
			}
			else
			{
				rtvs[i] = D3D12NullDescriptor<DescriptorType::RenderTargetView, DescriptorUnderlyingType::Texture2D>::Get().GetCpuHandle().At();
			}
		}

		D3D12_CPU_DESCRIPTOR_HANDLE dsv = {};
		if ( auto d3d12DSV = static_cast<D3D12DepthStencilView*>( depthStencil ) )
		{
			dsv = d3d12DSV->GetCpuHandle().At();
		}

		std::copy( pRenderTargets, pRenderTargets + renderTargetCount, std::begin( m_rtvs ) );
		m_dsv = depthStencil;
		commandList.OMSetRenderTargets( renderTargetCount, rtvs, false, depthStencil ? &dsv : nullptr );
	}

	D3D12PipelineCache::D3D12PipelineCache()
		: m_allocatedIdentifiers( m_allocatedIdentifierAllocator )
		, m_allocatedInfos( m_allocatedInfoAllocator )
	{
	}

	void D3D12PipelineCache::RegisterRenderResource( GraphicsApiResource* resource )
	{
		if ( resource == nullptr )
		{
			return;
		}

		const AllocatedResourceInfo* resourceInfo = nullptr;

		if ( resource->IsBuffer() )
		{
			auto d3d12Buffer = static_cast<D3D12Buffer*>( resource );
			resourceInfo = &d3d12Buffer->GetResourceInfo();
		}
		else if ( resource->IsTexture() )
		{
			auto d3d12Texture = static_cast<D3D12Texture*>( resource );
			resourceInfo = &d3d12Texture->GetResourceInfo();
		}

		if ( ( resourceInfo == nullptr )
			|| resourceInfo->IsExternalResource() )
		{
			return;
		}

		const ID3D12Resource* d3d12Resource = resourceInfo->GetResource();
		if ( m_allocatedIdentifiers.find( d3d12Resource ) == std::end( m_allocatedIdentifiers ) )
		{
			m_allocatedIdentifiers.insert( d3d12Resource );
			m_allocatedInfos.emplace_back( *resourceInfo );
		}
	}

	void D3D12PipelineCache::RegisterRenderResource( IUnknown* resource )
	{
		m_residentResource.emplace_back( resource );
	}
}
