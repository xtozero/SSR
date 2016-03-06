#pragma once

#include "common.h"

#include <memory>

struct ID3D11RasterizerState;
struct ID3D11Device;
namespace Microsoft
{
	namespace WRL
	{
		template <typename T> class ComPtr;
	}
}

class IRasterizerStateFactory
{
public:
	virtual void LoadDesc( ) = 0;
	virtual Microsoft::WRL::ComPtr<ID3D11RasterizerState> GetRasterizerState( ID3D11Device* pDevice, const String& stateName ) = 0;

	virtual ~IRasterizerStateFactory( ) = default;
};

std::unique_ptr<IRasterizerStateFactory> CreateRasterizerStateFactory( );