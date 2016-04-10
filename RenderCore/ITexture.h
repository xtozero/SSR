#pragma once

#include "common.h"

struct ID3D11Device;
struct ID3D11Resource;
struct D3D11_TEXTURE2D_DESC;
namespace Microsoft
{
	namespace WRL
	{
		template <typename T> class ComPtr;
	}
}

enum class TEXTURE_TYPE
{
	TEXTURE_1D = 0,
	TEXTURE_2D,
	TEXTURE_3D
};

class ITexture
{
public:
	virtual bool LoadFromFile( ID3D11Device* pDevice, const String& fileName ) = 0;
	virtual ID3D11Resource* Get( ) const = 0;
	virtual bool Create( ID3D11Device* pDevice, const D3D11_TEXTURE2D_DESC& desc ) = 0;
	virtual TEXTURE_TYPE GetType( ) const = 0;
};