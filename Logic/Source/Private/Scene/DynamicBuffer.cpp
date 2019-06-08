#include "stdafx.h"
#include "Scene/DynamicBuffer.h"

#include "Core/GameLogic.h"

void CDynamicBuffer::OnDeviceRestore( CGameLogic& gameLogic )
{
	IRenderer& renderer = gameLogic.GetRenderer( );

	m_bufferHandle = CreateDynamicBuffer( renderer.GetResourceManager( ), m_bindType, m_capacity );
}

void CDynamicBuffer::Initialize( IResourceManager& resourceMgr, unsigned int bufferType, unsigned int bufferSize )
{
	resourceMgr.FreeResource( m_bufferHandle );
	m_bufferHandle = CreateDynamicBuffer( resourceMgr, bufferType, bufferSize );
	m_capacity = bufferSize;
	m_bindType = bufferType;
}

void CDynamicBuffer::Unmap( IRenderer& renderer )
{
	if ( m_lockedBuffer )
	{
		renderer.UnLockBuffer( m_bufferHandle );
		m_lockedBuffer = nullptr;
	}
}

RE_HANDLE CDynamicBuffer::CreateDynamicBuffer( IResourceManager& resourceMgr, unsigned int bufferType, unsigned int bufferSize )
{
	using namespace RESOURCE_ACCESS_FLAG;

	BUFFER_TRAIT trait = {
		bufferSize,
		1U,
		GPU_READ | CPU_WRITE,
		bufferType,
		0U,
		nullptr,
		0U,
		0U
	};

	RE_HANDLE buffer = resourceMgr.CreateBuffer( trait );
	if ( buffer == RE_HANDLE_TYPE::INVALID_HANDLE )
	{
		__debugbreak( );
	}

	return buffer;
}