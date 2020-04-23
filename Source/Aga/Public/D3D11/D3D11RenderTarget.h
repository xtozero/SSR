#pragma once

#include "D3D11ResourceInterface.h"

class CD3D11Texture2D;

class CD3D11RenderTarget : public GraphicsViewBase<ID3D11RenderTargetView>, public FrameBufferDependent
{
public:
	virtual void InitResource( ) override;

	CD3D11RenderTarget( const RefHandle<CD3D11Texture2D>& texture, const TEXTURE_TRAIT* traitOrNull );
	~CD3D11RenderTarget( ) = default;
	CD3D11RenderTarget( const CD3D11RenderTarget& ) = delete;
	CD3D11RenderTarget( CD3D11RenderTarget&& ) = default;
	CD3D11RenderTarget& operator=( const CD3D11RenderTarget& ) = delete;
	CD3D11RenderTarget& operator=( CD3D11RenderTarget&& ) = default;

private:
	RefHandle<CD3D11Texture2D> m_texture;
	D3D11_RENDER_TARGET_VIEW_DESC m_desc;
};