#pragma once

#include "INotifyGraphicsDevice.h"
//#include "Render/IRenderer.h"
//#include "Render/IRenderResourceManager.h"
#include "SizedTypes.h"

#include <cstddef>

class CDynamicBuffer : public IGraphicsDeviceNotify
{
public:
	virtual void OnDeviceRestore( CGameLogic& gameLogic ) override;

	//void Initialize( IResourceManager& resourceMgr, uint32 bufferType, uint32 bufferSize );
	
	//RE_HANDLE GetHandle( ) const { return m_bufferHandle; }

	//template <typename T>
	//T* Map( IRenderer& renderer, uint32 size )
	//{
	//	if ( m_capacity < size )
	//	{
	//		if ( m_lockedBuffer )
	//		{
	//			Unmap( renderer );
	//		}

	//		IResourceManager& resourceMgr = renderer.GetResourceManager( );
	//		resourceMgr.FreeResource( m_bufferHandle );
	//		m_bufferHandle = CreateDynamicBuffer( renderer.GetResourceManager( ), m_bindType, size );
	//		m_capacity = size;
	//	}

	//	if ( m_lockedBuffer == nullptr )
	//	{
	//		m_lockedBuffer = static_cast<T*>( renderer.LockBuffer( m_bufferHandle ) );
	//	}

	//	return static_cast<T*>( m_lockedBuffer );
	//}

	//void Unmap( IRenderer& renderer );

private:
	//RE_HANDLE CreateDynamicBuffer( IResourceManager& resourceMgr, uint32 bufferType, uint32 bufferSize );

	//RE_HANDLE m_bufferHandle = RE_HANDLE::InValidHandle( );
	uint32 m_capacity = 0;
	//uint32 m_bindType = RESOURCE_BIND_TYPE::VERTEX_BUFFER;
	void* m_lockedBuffer = nullptr;
};