#pragma once

#include "common.h"

#include <memory>
#include <wrl/client.h>

struct ID3D11DepthStencilState;
struct ID3D11Device;

class IDepthStencilStateFactory
{
public:
	virtual void LoadDesc( ) = 0;
	virtual Microsoft::WRL::ComPtr<ID3D11DepthStencilState> GetDepthStencilState( ID3D11Device* pDevice, const String& stateName ) = 0;

	virtual ~IDepthStencilStateFactory( ) = default;
};

std::unique_ptr<IDepthStencilStateFactory> CreateDepthStencailStateFactory( );