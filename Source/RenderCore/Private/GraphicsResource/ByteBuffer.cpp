#include "ByteBuffer.h"

#include "CommandList.h"
#include "ComputePipelineState.h"
#include "GlobalShaders.h"
#include "RenderGraph.h"
#include "ShaderParameterUtils.h"
#include "TaskScheduler.h"
#include "UploadBuffer.h"

namespace rendercore
{
	REGISTER_GLOBAL_SHADER( DistributionCopyCS, "./Assets/Shaders/Common/CS_DistributionCopy.asset" );

	GpuMemcpy::GpuMemcpy( uint32 numUpload, uint32 sizePerFloat4, UploadBuffer& src, UploadBuffer& distributer ) : m_src( src ), m_distributer( distributer ), m_sizePerFloat4( sizePerFloat4 )
	{
		m_src.Resize( m_sizePerFloat4 * numUpload, nullptr );
		m_distributer.Resize( m_sizePerFloat4 * numUpload, nullptr );

		m_pUploadData = m_src.Lock<char>();
		m_pDistributionData = m_distributer.Lock<uint32>();
	}

	void GpuMemcpy::Add( const char* data, uint32 dstIndex )
	{
		uint32 elementSize = m_src.ElementSize();

		std::memcpy( m_pUploadData, data, elementSize * m_sizePerFloat4 );

		for ( uint32 i = 0; i < m_sizePerFloat4; ++i )
		{
			m_pDistributionData[i] = static_cast<uint32>( dstIndex * m_sizePerFloat4 + i );
		}

		m_pUploadData += elementSize * m_sizePerFloat4;
		m_pDistributionData += m_sizePerFloat4;
		m_distributionCount += m_sizePerFloat4;
	}

	void GpuMemcpy::Upload( RenderGraph& renderGraph, RefHandle<agl::Buffer> destBuffer )
	{
		assert( IsInRenderThread() );

		BEGIN_RG_RESOURCE_STRUCT( GpuMemcpyPassResource )
			DECLARE_RG_BUFFER_UAV( destBuffer )
		END_RG_RESOURCE_STRUCT();

		auto rgDestBuffer = renderGraph.RegisterExternalResource( destBuffer.Get() );

		GpuMemcpyPassResource passResource = {
			.m_destBuffer = rgDestBuffer,
		};

		uint32 numDistribution = m_distributionCount;
		agl::Buffer* srcBuffer = m_src.Resource();
		agl::Buffer* distributerBuffer = m_distributer.Resource();

		renderGraph.AddPass(
			passResource,
			[passResource, numDistribution, srcBuffer, distributerBuffer]( ComputeCommandList& commandList )
			{
				agl::Buffer* destBuffer = passResource.m_destBuffer->Get();

				DistributionCopyCS distributionCopyCS;

				RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( distributionCopyCS );

				commandList.BindPipelineState( pso.Get() );

				agl::ShaderBindings shaderBindings = CreateShaderBindings( distributionCopyCS );
				SetShaderValue( commandList, distributionCopyCS.NumDistribution(), numDistribution );
				BindResource( shaderBindings, distributionCopyCS.Src(), srcBuffer );
				BindResource( shaderBindings, distributionCopyCS.Distributer(), distributerBuffer );
				BindResource( shaderBindings, distributionCopyCS.Dest(), destBuffer );

				commandList.BindShaderResources( shaderBindings );

				uint32 threadGroup = ( ( numDistribution + DistributionCopyCS::ThreadGroupX - 1 ) / DistributionCopyCS::ThreadGroupX );

				commandList.Dispatch( threadGroup, 1 );
			} );

		m_src.Unlock();
		m_distributer.Unlock();

		m_pUploadData = nullptr;
		m_pDistributionData = nullptr;
	}

	agl::Buffer* ByteBuffer::Resource() const
	{
		return m_buffer.Get();
	}

	ByteBuffer::ByteBuffer( uint32 size, agl::ResourceState initialState, const void* initData, bool isDynamic )
		: m_isDynamic( isDynamic )
	{
		InitResource( size, initialState, initData );
	}

	void ByteBuffer::InitResource( uint32 size, agl::ResourceState initialState, const void* initData )
	{
		auto accessFlag = m_isDynamic ? agl::ResourceAccessFlag::Upload : agl::ResourceAccessFlag::Default;

		agl::BufferTrait trait = {
			.m_stride = 1,
			.m_count = size,
			.m_access = accessFlag,
			.m_bindType = agl::ResourceBindType::ShaderResource,
			.m_miscFlag = agl::ResourceMisc::BufferAllowRawViews,
			.m_format = agl::ResourceFormat::Unknown
		};

		m_buffer = agl::Buffer::Create( trait, "Byte", initialState, initData );
		EnqueueRenderTask(
			[buffer = m_buffer]()
			{
				buffer->Init();
			} );
	}
}
