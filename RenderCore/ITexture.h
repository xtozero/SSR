#pragma once

#include <wrl/client.h>
#include "common.h"

struct ID3D11Device;
struct ID3D11ShaderResourceView;

class ITexture
{
public:
	virtual bool LoadTexture( ID3D11Device* pDevice, const String& fileName ) = 0;
	virtual ID3D11ShaderResourceView* GetResource( ) = 0;
	
	virtual ~ITexture( ) = default;

protected:
	ITexture( ) = default;
};