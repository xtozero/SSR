#include "stdafx.h"
#include "Scene/ComputeShaderUnitTest.h"

#include "Core/GameLogic.h"
#include "Render/IRenderer.h"
#include "Render/IRenderResourceManager.h"

#include <vector>

void CComputeShaderUnitTest::RunUnitTest( CGameLogic& gameLogic )
{
	using namespace RESOURCE_ACCESS_FLAG;

	constexpr int DATA_SIZE = 6220800;

	std::vector<float> data;
	data.resize( DATA_SIZE );

	for ( auto& elem : data )
	{
		elem = 1;
	}

	IRenderer& renderer = gameLogic.GetRenderer( );

	BUFFER_TRAIT bufferTrait = {
		sizeof( float ),
		DATA_SIZE,
		GPU_READ | GPU_WRITE,
		RESOURCE_BIND_TYPE::UNORDERED_ACCESS | RESOURCE_BIND_TYPE::SHADER_RESOURCE,
		RESOURCE_MISC::BUFFER_STRUCTURED,
		data.data(),
		0,
		0
	};

	m_buffers[0] = renderer.CreateBuffer( bufferTrait );
	if ( m_buffers[0] == RE_HANDLE::InValidHandle( ) )
	{
		__debugbreak( );
	}

	bufferTrait.m_srcData = nullptr;

	m_buffers[1] = renderer.CreateBuffer( bufferTrait );
	if ( m_buffers[1] == RE_HANDLE::InValidHandle( ) )
	{
		__debugbreak( );
	}

	bufferTrait.m_count = 1;
	bufferTrait.m_access = GPU_READ | GPU_WRITE | CPU_READ;
	bufferTrait.m_miscFlag = 0;
	bufferTrait.m_bindType = 0;

	m_readBackBuffers = renderer.CreateBuffer( bufferTrait );
	if ( m_readBackBuffers == RE_HANDLE::InValidHandle( ) )
	{
		__debugbreak( );
	}

	bufferTrait.m_stride = sizeof( int );
	bufferTrait.m_count = 4;
	bufferTrait.m_access = GPU_READ | CPU_WRITE;
	bufferTrait.m_bindType = RESOURCE_BIND_TYPE::CONSTANT_BUFFER;
	m_constantBuffer = renderer.CreateBuffer( bufferTrait );
	if ( m_constantBuffer == RE_HANDLE::InValidHandle( ) )
	{
		__debugbreak( );
	}

	IResourceManager& rscMgr = renderer.GetResourceManager( );
	m_srvs[0] = rscMgr.CreateBufferShaderResource( m_buffers[0], _T( "csInitBuffer" ) );
	if ( m_srvs[0] == RE_HANDLE::InValidHandle( ) )
	{
		__debugbreak( );
	}

	m_srvs[1] = rscMgr.CreateBufferShaderResource( m_buffers[1], _T( "csResultBuffer" ) );
	if ( m_srvs[1] == RE_HANDLE::InValidHandle( ) )
	{
		__debugbreak( );
	}

	m_uavs[0] = rscMgr.CreateBufferRandomAccess( m_buffers[0], _T( "csInitBuffer" ) );
	if ( m_uavs[0] == RE_HANDLE::InValidHandle( ) )
	{
		__debugbreak( );
	}

	m_uavs[1] = rscMgr.CreateBufferRandomAccess( m_buffers[1], _T( "csResultBuffer" ) );
	if ( m_uavs[1] == RE_HANDLE::InValidHandle( ) )
	{
		__debugbreak( );
	}

	m_computeShader = renderer.CreateComputeShader( _T( "./Shaders/csExperimental.cso" ), nullptr );
	if ( m_computeShader == RE_HANDLE::InValidHandle( ) )
	{
		__debugbreak( );
	}

	renderer.BindConstantBuffer( SHADER_TYPE::CS, 0, 1, &m_constantBuffer );

	renderer.BindShader( SHADER_TYPE::CS, m_computeShader );
	
	bool flag = false;
	unsigned int datacount = DATA_SIZE;
	do
	{
		flag = !flag;

		RE_HANDLE defaultHandle[] = { RE_HANDLE::InValidHandle( ) };
		renderer.BindShaderResource( SHADER_TYPE::CS, 0, 1, defaultHandle );
		renderer.BindRandomAccessResource( 0, 1, &m_uavs[flag ? 1 : 0] );
		renderer.BindShaderResource( SHADER_TYPE::CS, 0, 1, &m_srvs[flag ? 0 : 1] );

		int* dataSize = static_cast<int*>( renderer.LockBuffer( m_constantBuffer ) );
		if ( dataSize )
		{
			*dataSize = datacount;
			renderer.UnLockBuffer( m_constantBuffer );
		}
		else
		{
			__debugbreak( );
		}

		unsigned int threadgroup = ( datacount + 127 ) / 128;
		renderer.Dispatch( threadgroup, 1 );
		datacount = threadgroup;
	} while ( datacount > 1 );

	RESOURCE_REGION srcRegion = { 0, 0, 0, 0, 0, 0, 0 };
	RESOURCE_REGION destRegion = { 0, 0, sizeof(float), 0, 1, 0, 1 };
	rscMgr.CopyResource( m_readBackBuffers, &srcRegion, m_buffers[flag ? 1 : 0], &destRegion );

	float sum = 0;
	float* Result = static_cast<float*>( renderer.LockBuffer( m_readBackBuffers, BUFFER_LOCKFLAG::READ ) );
	if ( Result )
	{
		sum = *Result;
		renderer.UnLockBuffer( m_readBackBuffers );
	}

	if ( sum != DATA_SIZE )
	{
		__debugbreak( );
	}
}
