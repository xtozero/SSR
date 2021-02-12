//#pragma once
//
//#include "D3D11ResourceInterface.h"
//
//class CD3D11Texture2D;
//
//class CD3D11DepthStencil : public GraphicsViewBase<ID3D11DepthStencilView>, public FrameBufferDependent
//{
//public:
//	virtual void InitResource( ) override;
//	void Clear( unsigned int clearFlag, float depth, unsigned char stencil );
//
//	CD3D11DepthStencil( const RefHandle<CD3D11Texture2D>& texture, const TEXTURE_TRAIT* traitOrNull );
//	~CD3D11DepthStencil( ) = default;
//	CD3D11DepthStencil( const CD3D11DepthStencil& ) = delete;
//	CD3D11DepthStencil( CD3D11DepthStencil&& ) = default;
//	CD3D11DepthStencil& operator=( const CD3D11DepthStencil& ) = delete;
//	CD3D11DepthStencil& operator=( CD3D11DepthStencil&& ) = default;
//
//private:
//	RefHandle<CD3D11Texture2D> m_texture;
//	D3D11_DEPTH_STENCIL_VIEW_DESC m_desc;
//};