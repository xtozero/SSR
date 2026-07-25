#include "Canvas.h"

#include "../RenderResource/Canvas.h"
#include "TaskScheduler.h"

namespace rendercore
{
	agl::Canvas* Canvas::Resource()
	{
		return m_canvas.Get();
	}

	void Canvas::OnBeginFrameRendering()
	{
		if ( m_canvas.Get() )
		{
			m_canvas->OnBeginFrameRendering();
		}
	}

	void Canvas::OnEndFrameRendering()
	{
		if ( m_canvas.Get() )
		{
			m_canvas->OnEndFrameRendering();
		}
	}

	agl::DeviceError Canvas::Present( bool vSync, bool allowTearing )
	{
		if ( m_canvas.Get() )
		{
			return m_canvas->Present( vSync, allowTearing );
		}

		return agl::DeviceError::DeviceLost;
	}

	void Canvas::Clear()
	{
		if ( m_canvas.Get() )
		{
			m_canvas->Clear();
		}
	}

	void* Canvas::Handle() const
	{
		if ( m_canvas.Get() )
		{
			return m_canvas->Handle();
		}

		return nullptr;
	}

	RENDERCORE_DLL std::pair<uint32, uint32> Canvas::Size() const
	{
		if ( m_canvas.Get() )
		{
			return m_canvas->Size();
		}

		return {};
	}

	void Canvas::Resize( uint32 width, uint32 height )
	{
		if ( m_canvas.Get() )
		{
			m_canvas->Resize( width, height );
		}
	}

	agl::Texture* Canvas::Texture()
	{
		if ( m_canvas.Get() )
		{
			return m_canvas->Texture();
		}

		return nullptr;
	}

	uint32 Canvas::GetBackBufferIndex() const
	{
		if ( m_canvas.Get() )
		{
			return m_canvas->GetBackBufferIndex();
		}

		return 0;
	}

	Canvas::Canvas( uint32 width, uint32 height, const engine::PlatformWindowContext& windowCtx, agl::ResourceFormat format, const float4& clearColor )
	{
		m_canvas = agl::Canvas::Create( width, height, windowCtx, format, clearColor );
	}

	Canvas::~Canvas() = default;
}
