#pragma once

#include "common.h"
#include "../Engine/ScriptKeyHandler.h"

#include <memory>

struct D3D11_RASTERIZER_DESC;
struct ID3D11RasterizerState;
struct ID3D11Device;
namespace Microsoft
{
	namespace WRL
	{
		template <typename T> class ComPtr;
	}
}

class IRasterizerStateFactory : public CScriptKeyHandler<IRasterizerStateFactory>
{
public:
	virtual void LoadDesc( ) = 0;
	virtual Microsoft::WRL::ComPtr<ID3D11RasterizerState> GetRasterizerState( ID3D11Device* pDevice, const String& stateName ) = 0;
	virtual void AddRasterizerDesc( const String& descName, const D3D11_RASTERIZER_DESC& newDesc ) = 0;

	virtual ~IRasterizerStateFactory( ) = default;
};

std::unique_ptr<IRasterizerStateFactory> CreateRasterizerStateFactory( );