#include "stdafx.h"
#include "Viewport.h"

#include "../RenderResource/Viewport.h"
#include "AbstractGraphicsInterface.h"
#include "Canvas.h"
#include "InterfaceFactories.h"
#include "IRenderResourceManager.h"
#include "TaskScheduler.h"

namespace rendercore
{
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

	std::pair<uint32, uint32> Viewport::Size() const
	{
		if ( m_pViewport.Get() )
		{
			return m_pViewport->Size();
		}

		return {};
	}

	RENDERCORE_DLL std::pair<uint32, uint32> Viewport::SizeOnRenderThread() const
	{
		if ( m_pViewport.Get() )
		{
			return m_pViewport->SizeOnRenderThread();
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

	Viewport::Viewport( uint32 width, uint32 height, agl::ResourceFormat format, const float4& bgColor )
	{
		m_pViewport = agl::Viewport::Create( width, height, format, bgColor );
		EnqueueRenderTask(
			[viewport = m_pViewport]()
			{
				viewport->Init();
			} );
	}

	Viewport::Viewport( rendercore::Canvas& canvas )
	{
		m_pViewport = agl::Viewport::Create( *canvas.Resource() );
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
