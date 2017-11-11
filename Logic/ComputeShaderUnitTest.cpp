#include "stdafx.h"
#include "ComputeShaderUnitTest.h"

#include "GameLogic.h"

#include "../RenderCore/CommonRenderer/IBuffer.h"
#include "../RenderCore/CommonRenderer/IRenderer.h"
#include "../RenderCore/CommonRenderer/IRenderResource.h"
#include "../RenderCore/CommonRenderer/IRenderResourceManager.h"
#include "../RenderCore/CommonRenderer/IShader.h"

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
		RESOURCE_TYPE::UNORDERED_ACCESS | RESOURCE_TYPE::SHADER_RESOURCE,
		RESOURCE_MISC::BUFFER_STRUCTURED,
		data.data(),
		0,
		0
	};

	m_pBuffer[0] = renderer.CreateBuffer( bufferTrait );
	if ( m_pBuffer[0] == nullptr )
	{
		__debugbreak( );
	}

	bufferTrait.m_srcData = nullptr;

	m_pBuffer[1] = renderer.CreateBuffer( bufferTrait );
	if ( m_pBuffer[1] == nullptr )
	{
		__debugbreak( );
	}

	bufferTrait.m_count = 1;
	bufferTrait.m_access = GPU_READ | GPU_WRITE | CPU_READ;
	bufferTrait.m_miscFlag = 0;
	bufferTrait.m_bufferType = 0;

	m_pReadBackBuffer = renderer.CreateBuffer( bufferTrait );
	if ( m_pReadBackBuffer == nullptr )
	{
		__debugbreak( );
	}

	bufferTrait.m_stride = sizeof( int );
	bufferTrait.m_count = 4;
	bufferTrait.m_access = GPU_READ | CPU_WRITE;
	bufferTrait.m_bufferType = RESOURCE_TYPE::CONSTANT_BUFFER;
	m_pConstantBuffer = renderer.CreateBuffer( bufferTrait );
	if ( m_pConstantBuffer == nullptr )
	{
		__debugbreak( );
	}

	IResourceManager& rscMgr = renderer.GetResourceManager( );
	m_pSrv[0] = rscMgr.CreateShaderResource( *m_pBuffer[0], _T( "csInitBuffer" ) );
	if ( m_pSrv[0] == nullptr )
	{
		__debugbreak( );
	}

	m_pSrv[1] = rscMgr.CreateShaderResource( *m_pBuffer[1], _T( "csResultBuffer" ) );
	if ( m_pSrv[1] == nullptr )
	{
		__debugbreak( );
	}

	m_pUav[0] = rscMgr.CreateRandomAccessResource( *m_pBuffer[0], _T( "csInitBuffer" ) );
	if ( m_pUav[0] == nullptr )
	{
		__debugbreak( );
	}

	m_pUav[1] = rscMgr.CreateRandomAccessResource( *m_pBuffer[1], _T( "csResultBuffer" ) );
	if ( m_pUav[1] == nullptr )
	{
		__debugbreak( );
	}

	m_pComputeShader = renderer.CreateComputeShader( _T( "../bin/Shader/csExperimental.cso" ), nullptr );
	if ( m_pComputeShader == nullptr )
	{
		__debugbreak( );
	}

	m_pConstantBuffer->SetCSBuffer( 0 );

	m_pComputeShader->SetShader( );
	
	bool flag = false;
	unsigned int datacount = DATA_SIZE;
	do
	{
		flag = !flag;

		m_pComputeShader->SetShaderResource( 0, nullptr );
		m_pComputeShader->SetRandomAccessResource( 0, m_pUav[flag ? 1 : 0] );
		m_pComputeShader->SetShaderResource( 0, m_pSrv[flag ? 0 : 1] );

		int* dataSize = static_cast<int*>( m_pConstantBuffer->LockBuffer( ) );
		if ( dataSize )
		{
			*dataSize = datacount;
			m_pConstantBuffer->UnLockBuffer( );
		}
		else
		{
			__debugbreak( );
		}

		unsigned int threadgroup = ( datacount + 127 ) / 128;
		m_pComputeShader->Dispatch( threadgroup, 1 );
		datacount = threadgroup;
	} while ( datacount > 1 );

	RESOURCE_REGION srcRegion = { 0, 0, 0, 0, 0, 0, 0 };
	RESOURCE_REGION destRegion = { 0, 0, sizeof(float), 0, 1, 0, 1 };
	rscMgr.CopyResource( *m_pReadBackBuffer, &srcRegion, *m_pBuffer[flag ? 1 : 0], &destRegion );

	float sum = 0;
	float* Result = static_cast<float*>( m_pReadBackBuffer->LockBuffer( BUFFER_LOCKFLAG::READ ) );
	if ( Result )
	{
		sum = *Result;
		m_pReadBackBuffer->UnLockBuffer( );
	}

	if ( sum != DATA_SIZE )
	{
		__debugbreak( );
	}
}
