#include "stdafx.h"
#include "ByteBuffer.h"

#include "AbstractGraphicsInterface.h"
#include "GlobalShaders.h"
#include "IAga.h"
#include "TaskScheduler.h"
#include "UploadBuffer.h"

REGISTER_GLOBAL_SHADER( DistributionCopyCS, "./Assets/Shaders/CS_DistributionCopy.asset" );

DistributionCopyCS::DistributionCopyCS( )
{
	m_shader = static_cast<ComputeShader*>( GetGlobalShader<DistributionCopyCS>( ) );

	if ( m_shader )
	{
		m_numDistribution.Bind( m_shader->ParameterMap( ), "numDistribution" );
		m_src.Bind( m_shader->ParameterMap( ), "src" );
		m_distributer.Bind( m_shader->ParameterMap( ), "distributer" );
		m_dest.Bind( m_shader->ParameterMap( ), "dest" );
	}
}

GpuMemcpy::GpuMemcpy( uint32 numUpload, uint32 sizePerFloat4, UploadBuffer& src, UploadBuffer& distributer ) : m_sizePerFloat4( sizePerFloat4 ), m_src( src ), m_distributer( distributer )
{
	m_src.Resize( m_sizePerFloat4 * numUpload, nullptr );
	m_distributer.Resize( m_sizePerFloat4 * numUpload, nullptr );

	m_pUploadData = m_src.Lock<char>( );
	m_pDistributionData = m_distributer.Lock<uint32>( );
}

void GpuMemcpy::Add( const char* data, uint32 dstIndex )
{
	uint32 elementSize = m_src.ElementSize( );

	std::memcpy( m_pUploadData, data, elementSize * m_sizePerFloat4 );

	for ( uint32 i = 0; i < m_sizePerFloat4; ++i )
	{
		m_pDistributionData[i] = static_cast<uint32>( dstIndex * m_sizePerFloat4 + i );
	}

	m_pUploadData += elementSize * m_sizePerFloat4;
	m_pDistributionData += m_sizePerFloat4;
	m_distributionCount += m_sizePerFloat4;
}

void GpuMemcpy::Upload( aga::Buffer* destBuffer )
{
	assert( IsInRenderThread( ) );

	m_src.Unlock( );
	m_distributer.Unlock( );

	m_pUploadData = nullptr;
	m_pDistributionData = nullptr;

	DistributionCopyCS computeShader;
	ComputeShader& cs = *computeShader.Shader( );

	auto& graphicsInterface = GraphicsInterface( );

	graphicsInterface.BindShader( cs );

	graphicsInterface.SetShaderValue( cs, computeShader.m_numDistribution, m_distributionCount );
	graphicsInterface.BindShaderParameter( cs, computeShader.m_src, m_src.Resource( ) );
	graphicsInterface.BindShaderParameter( cs, computeShader.m_distributer, m_distributer.Resource( ) );
	graphicsInterface.BindShaderParameter( cs, computeShader.m_dest, destBuffer );

	uint32 threadGroup = ( ( m_distributionCount + DistributionCopyCS::ThreadGroupX - 1 ) / DistributionCopyCS::ThreadGroupX );

	graphicsInterface.Dispatch( threadGroup, 1 );

	graphicsInterface.BindShaderParameter( cs, computeShader.m_dest, nullptr );
}

