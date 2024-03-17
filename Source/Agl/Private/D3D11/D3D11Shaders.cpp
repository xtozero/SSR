#include "D3D11Shaders.h"

#include "D3D11Api.h"

#include "ShaderParameterMap.h"

namespace agl
{
	void D3D11VertexShader::InitResource()
	{
		[[maybe_unused]] bool result = SUCCEEDED( D3D11Device().CreateVertexShader( m_byteCode, m_byteCodeSize, nullptr, &m_pResource ) );
		assert( result );
	}

	void D3D11VertexShader::FreeResource()
	{
		if ( m_pResource )
		{
			m_pResource->Release();
			m_pResource = nullptr;
		}
	}

	void D3D11GeometryShader::InitResource()
	{
		[[maybe_unused]] bool result = SUCCEEDED( D3D11Device().CreateGeometryShader( m_byteCode, m_byteCodeSize, nullptr, &m_pResource ) );
		assert( result );
	}

	void D3D11GeometryShader::FreeResource()
	{
		if ( m_pResource )
		{
			m_pResource->Release();
			m_pResource = nullptr;
		}
	}

	void D3D11PixelShader::InitResource()
	{
		[[maybe_unused]] bool result = SUCCEEDED( D3D11Device().CreatePixelShader( m_byteCode, m_byteCodeSize, nullptr, &m_pResource ) );
		assert( result );
	}

	void D3D11PixelShader::FreeResource()
	{
		if ( m_pResource )
		{
			m_pResource->Release();
			m_pResource = nullptr;
		}
	}

	void D3D11ComputeShader::InitResource()
	{
		[[maybe_unused]] bool result = SUCCEEDED( D3D11Device().CreateComputeShader( m_byteCode, m_byteCodeSize, nullptr, &m_pResource ) );
		assert( result );
	}

	void D3D11ComputeShader::FreeResource()
	{
		if ( m_pResource )
		{
			m_pResource->Release();
			m_pResource = nullptr;
		}
	}
}
