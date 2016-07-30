#pragma once

#include "common.h"

#include <memory>
#include <wrl/client.h>

struct ID3D10Blob;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;
class IShaderResource;
class IBuffer;

class CShaderByteCode
{
public:
	explicit CShaderByteCode( const size_t size );
	CShaderByteCode( const CShaderByteCode& byteCode ) = delete;
	CShaderByteCode( CShaderByteCode&& byteCode );
	CShaderByteCode& operator=( const CShaderByteCode& byteCode ) = delete;
	CShaderByteCode& operator=( CShaderByteCode&& byteCode );
	~CShaderByteCode( );

	BYTE* GetBufferPointer( ) const { return m_buffer; }
	size_t GetBufferSize( ) const { return m_size; }
private:
	BYTE* m_buffer;
	size_t m_size;
};

class IShader
{
public:
	bool CreateShader( ID3D11Device* pDevice, const TCHAR* pFilePath, const char* pProfile );

	virtual void SetShader ( ID3D11DeviceContext* pDeviceContext ) = 0;
	virtual void SetShaderResource( ID3D11DeviceContext* pDeviceContext, UINT slot, const IShaderResource* pResource ) = 0;
	virtual void SetConstantBuffer( ID3D11DeviceContext* pDeviceContext, UINT slot, const IBuffer* pBuffer ) = 0;

protected:
	virtual bool CreateShaderInternal( ID3D11Device* pDevice, const void* byteCodePtr, const size_t byteCodeSize ) = 0;

protected:
	Microsoft::WRL::ComPtr<ID3D10Blob> GetShaderBlob( const TCHAR* pFilePath, const char* pProfile );
	CShaderByteCode GetCompiledByteCode( const TCHAR* pFilePath );

	IShader () = default;

public:
	virtual ~IShader( ) = default;
};