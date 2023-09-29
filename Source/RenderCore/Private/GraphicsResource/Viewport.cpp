#include "stdafx.h"
#include "Viewport.h"

#include "../RenderResource/Viewport.h"
#include "AbstractGraphicsInterface.h"
#include "InterfaceFactories.h"
#include "IRenderResourceManager.h"
#include "TaskScheduler.h"

namespace rendercore
{
	void Viewport::OnBeginFrameRendering()
	{
		if ( m_pViewport.Get() )
		{
			m_pViewport->OnBeginFrameRendering();
		}
	}

	void Viewport::OnEndFrameRendering()
	{
		if ( m_pViewport.Get() )
		{
			m_pViewport->OnEndFrameRendering();
		}
	}

	agl::DeviceError Viewport::Present( bool vSync )
	{
		if ( m_pViewport.Get() )
		{
			return m_pViewport->Present( vSync );
		}

		return agl::DeviceError::DeviceLost;
	}

	void Viewport::Clear( const float( &color )[4] )
	{
		if ( m_pViewport.Get() )
		{
			m_pViewport->Clear( color );
		}
	}

	void Viewport::Bind( agl::ICommandListBase& commandList ) const
	{
		if ( m_pViewport.Get() )
		{
			m_pViewport->Bind( commandList );
		}
	}

	void* Viewport::Handle() const
	{
		if ( m_pViewport.Get() )
		{
			return m_pViewport->Handle();
		}

		return nullptr;
	}

	std::pair<uint32, uint32> Viewport::Size() const
	{
		if ( m_pViewport.Get() )
		{
			return m_pViewport->Size();
		}

		return {};
	}

	void Viewport::Resize( const std::pair<uint32, uint32>& newSize )
	{
		if ( m_pViewport.Get() )
		{
			m_pViewport->Resize( newSize );
		}
	}

	agl::Texture* Viewport::Texture()
	{
		if ( m_pViewport.Get() )
		{
			return m_pViewport->Texture();
		}

		return nullptr;
	}

	agl::Texture* Viewport::Canvas()
	{
		if ( m_pViewport.Get() )
		{
			return m_pViewport->Canvas();
		}

		return nullptr;
	}

	Viewport::Viewport( uint32 width, uint32 height, HWND hWnd, agl::ResourceFormat format, const float4& bgColor, bool useDedicateTexture )
	{
		m_pViewport = agl::Viewport::Create( width, height, hWnd, format, bgColor, useDedicateTexture );
		EnqueueRenderTask(
			[viewport = m_pViewport]()
			{
				viewport->Init();
			} );
	}

	Viewport::~Viewport()
	{
	}
}
