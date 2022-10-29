#include "D3D12Viewport.h"

#include "ICommandList.h"

namespace agl
{
	DeviceError D3D12Viewport::Present( bool vSync )
	{
		return DeviceError();
	}

	void D3D12Viewport::Clear( const float( &clearColor )[4] )
	{
	}

	void D3D12Viewport::Bind( ICommandList& commandList ) const
	{
		CubeArea<float> viewport{ 0.f, 0.f, static_cast<float>( m_width ), static_cast<float>( m_height ), 0.f, 1.f };
		commandList.SetViewports( 1, &viewport );

		RectangleArea<int32> rect{ 0L, 0L, static_cast<int32>( m_width ), static_cast<int32>( m_height ) };
		commandList.SetScissorRects( 1, &rect );
	}

	std::pair<uint32, uint32> D3D12Viewport::Size() const
	{
		return { m_width, m_height };
	}

	void D3D12Viewport::Resize( const std::pair<uint32, uint32>& newSize )
	{
	}

	agl::Texture* D3D12Viewport::Texture()
	{
		return nullptr;
	}

	D3D12Viewport::D3D12Viewport( uint32 width, uint32 height, void* hWnd, DXGI_FORMAT format ) :
		m_width( width ), m_height( height ), m_hWnd( hWnd ), m_format( format )
	{
	}

	void D3D12Viewport::InitResource()
	{
	}

	void D3D12Viewport::FreeResource()
	{
	}
}
