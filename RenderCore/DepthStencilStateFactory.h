#pragma once

#include "common.h"

#include <memory>

struct ID3D11DepthStencilState;
struct ID3D11Device;
namespace Microsoft
{
	namespace WRL
	{
		template <typename T> class ComPtr;
	}
}

class IDepthStencilStateFactory
{
public:
	virtual void LoadDesc( ) = 0;
	virtual Microsoft::WRL::ComPtr<ID3D11DepthStencilState> GetDepthStencilState( ID3D11Device* pDevice, const String& stateName ) = 0;

	virtual ~IDepthStencilStateFactory( ) = default;
};

std::unique_ptr<IDepthStencilStateFactory> CreateDepthStencailStateFactory( );