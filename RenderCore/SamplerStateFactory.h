#pragma once

#include "common.h"

#include <memory>

struct ID3D11SamplerState;
struct ID3D11Device;
namespace Microsoft
{
	namespace WRL
	{
		template <typename T> class ComPtr;
	}
}

class ISamplerStateFactory
{
public:
	virtual void LoadDesc( ) = 0;
	virtual Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSamplerState( ID3D11Device* pDevice, const String& stateName ) = 0;

	virtual ~ISamplerStateFactory( ) = default;
};

std::unique_ptr<ISamplerStateFactory> CreateSamplerStateFactory( );