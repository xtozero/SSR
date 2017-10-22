#pragma once

#include "../CommonRenderer/IRenderResource.h"

#include <wrl/client.h>

struct ID3D11Device;
struct ID3D11Resource;
struct ID3D11Texture1D;
struct ID3D11Texture2D;
struct ID3D11Texture3D;

class CD3D11Texture1D : public ITexture
{
public:
	bool SetTexture( Microsoft::WRL::ComPtr<ID3D11Resource>& pTexture );
	bool Create( ID3D11Device& device, const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr );
	bool LoadFromFile( ID3D11Device& device, const TCHAR* filePath );

	virtual void* Get( ) const override;
	virtual TEXTURE_TYPE GetType( ) const override { return TEXTURE_TYPE::TEXTURE_1D; }

private:
	Microsoft::WRL::ComPtr<ID3D11Texture1D> m_pTexture;
};

class CD3D11Texture2D : public ITexture
{
public:
	bool SetTexture( Microsoft::WRL::ComPtr<ID3D11Resource>& pTexture );
	bool Create( ID3D11Device& device, const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr );
	bool LoadFromFile( ID3D11Device& device, const TCHAR* filePath );

	virtual void* Get( ) const override;
	virtual TEXTURE_TYPE GetType( ) const override { return TEXTURE_TYPE::TEXTURE_2D; }

private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTexture;
};

class CD3D11Texture3D : public ITexture
{
public:
	bool SetTexture( Microsoft::WRL::ComPtr<ID3D11Resource>& pTexture );
	bool Create( ID3D11Device& device, const TEXTURE_TRAIT& trait, const RESOURCE_INIT_DATA* initData = nullptr );
	bool LoadFromFile( ID3D11Device& device, const TCHAR* filePath );

	virtual void* Get( ) const override;
	virtual TEXTURE_TYPE GetType( ) const override { return TEXTURE_TYPE::TEXTURE_3D; }

private:
	Microsoft::WRL::ComPtr<ID3D11Texture3D> m_pTexture;
};
