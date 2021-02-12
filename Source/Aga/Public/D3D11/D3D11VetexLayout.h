#pragma once

#include "D3D11ResourceInterface.h"
#include "VertexInputLayout.h"

//class CD3D11VertexShader;
class D3D11VertexShader;

//class CD3D11VertexLayout : public DeviceDependantBase<ID3D11InputLayout>
//{
//public:
//	virtual void InitResource( ) override;
//
//	CD3D11VertexLayout( const RefHandle<CD3D11VertexShader>& vs, const VertexLayoutDescElem* layout, int layoutSize );
//	CD3D11VertexLayout( const CD3D11VertexLayout& ) = delete;
//	CD3D11VertexLayout( CD3D11VertexLayout&& ) = default;
//	CD3D11VertexLayout& operator=( const CD3D11VertexLayout& ) = delete;
//	CD3D11VertexLayout& operator=( CD3D11VertexLayout&& ) = default;
//	~CD3D11VertexLayout( );
//
//private:
//	D3D11_INPUT_ELEMENT_DESC* m_imputDesc;
//	unsigned int m_layoutSize;
//	RefHandle<CD3D11VertexShader> m_vs;
//};

class D3D11VertexLayout : public VertexLayout
{
public:
	D3D11VertexLayout( const D3D11VertexShader* vs, const VertexLayoutDesc& layoutDesc );
	D3D11VertexLayout( const D3D11VertexLayout& ) = delete;
	D3D11VertexLayout( D3D11VertexLayout&& ) = default;
	D3D11VertexLayout& operator=( const D3D11VertexLayout& ) = delete;
	D3D11VertexLayout& operator=( D3D11VertexLayout&& ) = default;
	~D3D11VertexLayout( );

private:
	virtual void InitResource( ) override { }
	virtual void FreeResource( ) override;

	D3D11_INPUT_ELEMENT_DESC* m_inputDesc = nullptr;
	std::size_t m_layoutDescSize = 0;
	ID3D11InputLayout* m_pResource = nullptr;
};