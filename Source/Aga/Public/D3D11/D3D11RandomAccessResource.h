#pragma once

#include "D3D11ResourceInterface.h"

#include "D3D11Buffer.h"
#include "D3D11Textures.h"

class CD3D11RandomAccessResource : public GraphicsViewBase<ID3D11UnorderedAccessView>, public FrameBufferDependent
{
public:
	virtual void InitResource( ) override;

	CD3D11RandomAccessResource( const RefHandle<CD3D11Texture1D>& texture, const TEXTURE_TRAIT* traitOrNull );
	CD3D11RandomAccessResource( const RefHandle<CD3D11Texture2D>& texture, const TEXTURE_TRAIT* traitOrNull );
	CD3D11RandomAccessResource( const RefHandle<CD3D11Buffer>& buffer, const BUFFER_TRAIT* traitOrNull );
	~CD3D11RandomAccessResource( ) = default;
	CD3D11RandomAccessResource( const CD3D11RandomAccessResource& ) = delete;
	CD3D11RandomAccessResource( CD3D11RandomAccessResource&& ) = default;
	CD3D11RandomAccessResource& operator=( const CD3D11RandomAccessResource& ) = delete;
	CD3D11RandomAccessResource& operator=( CD3D11RandomAccessResource&& ) = default;

private:
	RefHandle<IGraphicsResource> m_resource;
	D3D11_UNORDERED_ACCESS_VIEW_DESC m_desc;
};