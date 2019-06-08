#pragma once

#include "INotifyGraphicsDevice.h"
#include "Math/CXMFloat.h"
#include "Render/IRenderer.h"
#include "Render/IRenderResourceManager.h"

class CDynamicBuffer : public IGraphicsDeviceNotify
{
public:
	virtual void OnDeviceRestore( CGameLogic& gameLogic ) override;

	void Initialize( IResourceManager& resourceMgr, unsigned int bufferType, unsigned int bufferSize );
	
	RE_HANDLE GetHandle( ) const { return m_bufferHandle; }

	template <typename T>
	T* Map( IRenderer& renderer, unsigned int size )
	{
		if ( m_capacity < size )
		{
			if ( m_lockedBuffer )
			{
				Unmap( renderer );
			}

			IResourceManager& resourceMgr = renderer.GetResourceManager( );
			resourceMgr.FreeResource( m_bufferHandle );
			m_bufferHandle = CreateDynamicBuffer( renderer.GetResourceManager( ), m_bindType, size );
			m_capacity = size;
		}

		if ( m_lockedBuffer == nullptr )
		{
			m_lockedBuffer = static_cast<T*>( renderer.LockBuffer( m_bufferHandle ) );
		}

		return static_cast<T*>( m_lockedBuffer );
	}

	void Unmap( IRenderer& renderer );

private:
	RE_HANDLE CreateDynamicBuffer( IResourceManager& resourceMgr, unsigned int bufferType, unsigned int bufferSize );

	RE_HANDLE m_bufferHandle = RE_HANDLE_TYPE::INVALID_HANDLE;
	size_t m_capacity = 0;
	unsigned int m_bindType = RESOURCE_BIND_TYPE::VERTEX_BUFFER;
	void* m_lockedBuffer = nullptr;
};