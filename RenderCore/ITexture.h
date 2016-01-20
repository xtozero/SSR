#pragma once

#include <wrl/client.h>
#include "../shared/Util.h"

struct ID3D11Device;
struct ID3D11ShaderResourceView;

class ITexture
{
public:
	virtual bool LoadTexture( ID3D11Device* pDevice, const String& fileName ) = 0;
	virtual ID3D11ShaderResourceView* GetResource( ) = 0;
};