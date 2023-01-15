#include "stdafx.h"
#include "ByteBuffer.h"

#include "CommandList.h"
#include "ComputePipelineState.h"
#include "GlobalShaders.h"
#include "ShaderParameterUtils.h"
#include "TaskScheduler.h"
#include "UploadBuffer.h"

namespace rendercore
{
	REGISTER_GLOBAL_SHADER( DistributionCopyCS, "./Assets/Shaders/Common/CS_DistributionCopy.asset" );

	DistributionCopyCS::DistributionCopyCS()
	{
		m_numDistribution.Bind( GetShader()->ParameterMap(), "NumDistribution" );
		m_src.Bind( GetShader()->ParameterMap(), "Src" );
		m_distributer.Bind( GetShader()->ParameterMap(), "Distributer" );
		m_dest.Bind( GetShader()->ParameterMap(), "Dest" );
	}

	GpuMemcpy::GpuMemcpy( uint32 numUpload, uint32 sizePerFloat4, UploadBuffer& src, UploadBuffer& distributer ) : m_sizePerFloat4( sizePerFloat4 ), m_src( src ), m_distributer( distributer )
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

	void GpuMemcpy::Upload( agl::Buffer* destBuffer )
	{
		assert( IsInRenderThread() );

		m_src.Unlock();
		m_distributer.Unlock();

		m_pUploadData = nullptr;
		m_pDistributionData = nullptr;

		DistributionCopyCS computeShader;
		ComputeShader& cs = *computeShader.GetShader();

		auto commandList = GetImmediateCommandList();

		agl::RefHandle<agl::ComputePipelineState> pso = PrepareComputePipelineState( cs.Resource() );

		commandList.BindPipelineState( pso.Get() );

		agl::ShaderBindings shaderBindings = CreateShaderBindings( &cs );
		SetShaderValue( computeShader.m_numDistribution, m_distributionCount );
		BindResource( shaderBindings, computeShader.m_src, m_src.Resource() );
		BindResource( shaderBindings, computeShader.m_distributer, m_distributer.Resource() );
		BindResource( shaderBindings, computeShader.m_dest, destBuffer );

		commandList.BindShaderResources( shaderBindings );

		uint32 threadGroup = ( ( m_distributionCount + DistributionCopyCS::ThreadGroupX - 1 ) / DistributionCopyCS::ThreadGroupX );

		commandList.Dispatch( threadGroup, 1 );
	}
}
