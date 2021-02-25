#include "stdafx.h"
#include "ByteBuffer.h"

#include "GlobalShaders.h"
#include "IAga.h"
#include "MultiThread/EngineTaskScheduler.h"
#include "RenderUtility/ShaderParameterUtil.h"
#include "UploadBuffer.h"

REGISTER_GLOBAL_SHADER( DistributionCopyCS, "./Assets/Shaders/CS_DistributionCopy.asset" );

DistributionCopyCS::DistributionCopyCS( )
{
	m_shader = static_cast<ComputeShader*>( GetGlobalShader<DistributionCopyCS>( ) );

	m_numDistribution.Bind( m_shader->ParameterMap( ), "numDistribution" );
	m_src.Bind( m_shader->ParameterMap( ), "src" );
	m_distributer.Bind( m_shader->ParameterMap( ), "distributer" );
	m_dest.Bind( m_shader->ParameterMap( ), "dest" );
}

GpuMemcpy::GpuMemcpy( std::size_t numUpload, UINT sizePerFloat4, UploadBuffer& src, UploadBuffer& distributer ) : m_sizePerFloat4( sizePerFloat4 ), m_src( src ), m_distributer( distributer )
{
	m_src.Resize( m_sizePerFloat4 * numUpload );
	m_distributer.Resize( m_sizePerFloat4 * numUpload );

	m_pUploadData = m_src.Lock<char>( );
	m_pDistributionData = m_distributer.Lock<UINT>( );
}

void GpuMemcpy::Add( const char* data, unsigned int dstIndex )
{
	std::size_t elementSize = m_src.ElementSize( );

	std::memcpy( m_pUploadData, data, elementSize * m_sizePerFloat4 );

	for ( std::size_t i = 0; i < m_sizePerFloat4; ++i )
	{
		m_pDistributionData[i] = static_cast<UINT>( dstIndex * m_sizePerFloat4 + i );
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

	GetAgaDelegator( ).BindShader( cs );

	SetShaderValue( cs, computeShader.m_numDistribution, m_distributionCount );
	BindShaderParameter( cs, computeShader.m_src, m_src );
	BindShaderParameter( cs, computeShader.m_distributer, m_distributer );
	BindShaderParameter( cs, computeShader.m_dest, destBuffer );

	UINT threadGroup = ( ( m_distributionCount + DistributionCopyCS::ThreadGroupX - 1 ) / DistributionCopyCS::ThreadGroupX );

	GetAgaDelegator( ).Dispatch( threadGroup, 1 );

	BindShaderParameter( cs, computeShader.m_dest, nullptr );
}

