#pragma once

#include "common.h"

struct ID3D11Device;
struct ID3D11Resource;
struct D3D11_SUBRESOURCE_DATA;
class TextureDescription;
namespace Microsoft
{
	namespace WRL
	{
		template <typename T> class ComPtr;
	}
}

enum class TEXTURE_TYPE
{
	TEXTURE_NONE = -1,
	TEXTURE_1D,
	TEXTURE_2D,
	TEXTURE_3D
};

class ITexture
{
public:
	virtual bool LoadFromFile( ID3D11Device* pDevice, const String& fileName ) = 0;
	virtual ID3D11Resource* Get( ) const = 0;
	virtual bool SetTexture( Microsoft::WRL::ComPtr<ID3D11Resource>& pTexture ) = 0;
	virtual bool Create( ID3D11Device* pDevice, const TextureDescription& desc, const D3D11_SUBRESOURCE_DATA* pInitialData = nullptr ) = 0;
	virtual TEXTURE_TYPE GetType( ) const = 0;
	virtual TextureDescription GetDesc( ) const = 0;
};