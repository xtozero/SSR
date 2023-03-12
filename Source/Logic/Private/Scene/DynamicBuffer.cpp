#include "stdafx.h"
#include "Scene/DynamicBuffer.h"

#include "Core/GameLogic.h"

#include <cassert>

void CDynamicBuffer::OnDeviceRestore( CGameLogic& gameLogic )
{
	//IRenderer& renderer = gameLogic.GetRenderer( );

	//assert( m_capacity <= UINT_MAX );
	//m_bufferHandle = CreateDynamicBuffer( renderer.GetResourceManager( ), m_bindType, static_cast<uint32>( m_capacity ) );
}

//void CDynamicBuffer::Initialize( IResourceManager& resourceMgr, uint32 bufferType, uint32 bufferSize )
//{
//	resourceMgr.FreeResource( m_bufferHandle );
//	m_bufferHandle = CreateDynamicBuffer( resourceMgr, bufferType, bufferSize );
//	m_capacity = bufferSize;
//	m_bindType = bufferType;
//}

//void CDynamicBuffer::Unmap( IRenderer& renderer )
//{
//	if ( m_lockedBuffer )
//	{
//		renderer.UnLockBuffer( m_bufferHandle );
//		m_lockedBuffer = nullptr;
//	}
//}

//RE_HANDLE CDynamicBuffer::CreateDynamicBuffer( IResourceManager& resourceMgr, uint32 bufferType, uint32 bufferSize )
//{
//	using namespace RESOURCE_ACCESS_FLAG;
//
//	BufferTrait trait = {
//		bufferSize,
//		1U,
//		GPU_READ | CPU_WRITE,
//		bufferType,
//		0U,
//		nullptr,
//		0U,
//		0U
//	};
//
//	RE_HANDLE buffer = resourceMgr.CreateBuffer( trait );
//	if ( buffer == RE_HANDLE::InValidHandle( ) )
//	{
//		__debugbreak( );
//	}
//
//	return buffer;
//}