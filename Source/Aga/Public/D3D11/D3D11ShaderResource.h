#pragma once

#include "D3D11ResourceInterface.h"

#include "D3D11Buffer.h"
#include "D3D11Textures.h"

class CD3D11ShaderResource : public GraphicsViewBase<ID3D11ShaderResourceView>, public FrameBufferDependent
{
public:
	virtual void InitResource( ) override;

	CD3D11ShaderResource( const RefHandle<CD3D11Texture1D>& texture, const TEXTURE_TRAIT* traitOrNull );
	CD3D11ShaderResource( const RefHandle<CD3D11Texture2D>& texture, const TEXTURE_TRAIT* traitOrNull );
	CD3D11ShaderResource( const RefHandle<CD3D11Texture3D>& texture, const TEXTURE_TRAIT* traitOrNull );
	CD3D11ShaderResource( const RefHandle<CD3D11Buffer>& buffer, const BUFFER_TRAIT* traitOrNull );
	~CD3D11ShaderResource( ) = default;
	CD3D11ShaderResource( const CD3D11ShaderResource& ) = delete;
	CD3D11ShaderResource( CD3D11ShaderResource&& ) = default;
	CD3D11ShaderResource& operator=( const CD3D11ShaderResource& ) = delete;
	CD3D11ShaderResource& operator=( CD3D11ShaderResource&& ) = default;

private:
	RefHandle<IGraphicsResource> m_resource;
	D3D11_SHADER_RESOURCE_VIEW_DESC m_desc;
};