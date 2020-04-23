#pragma once

#include "D3D11ResourceInterface.h"

class CD3D11VertexShader;

class CD3D11VertexLayout : public DeviceDependantBase<ID3D11InputLayout>
{
public:
	virtual void InitResource( ) override;

	CD3D11VertexLayout( const RefHandle<CD3D11VertexShader>& vs, const VERTEX_LAYOUT* layout, int layoutSize );
	CD3D11VertexLayout( const CD3D11VertexLayout& ) = delete;
	CD3D11VertexLayout( CD3D11VertexLayout&& ) = default;
	CD3D11VertexLayout& operator=( const CD3D11VertexLayout& ) = delete;
	CD3D11VertexLayout& operator=( CD3D11VertexLayout&& ) = default;
	~CD3D11VertexLayout( );

private:
	D3D11_INPUT_ELEMENT_DESC* m_imputDesc;
	unsigned int m_layoutSize;
	RefHandle<CD3D11VertexShader> m_vs;
};