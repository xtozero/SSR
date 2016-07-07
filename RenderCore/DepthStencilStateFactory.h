#pragma once

#include "common.h"
#include "IRenderState.h"

#include "../Engine/ScriptKeyHandler.h"

#include <memory>

struct D3D11_DEPTH_STENCIL_DESC;
struct ID3D11DepthStencilState;
struct ID3D11Device;
namespace Microsoft
{
	namespace WRL
	{
		template <typename T> class ComPtr;
	}
}

class IDepthStencilStateFactory : public CScriptKeyHandler<IDepthStencilStateFactory>
{
public:
	virtual void LoadDesc( ) = 0;
	virtual std::shared_ptr<IRenderState> GetDepthStencilState( ID3D11Device* pDevice, const String& stateName ) = 0;
	virtual void AddDepthStencilDesc( const String& descName, const D3D11_DEPTH_STENCIL_DESC& newDesc ) = 0;

	virtual ~IDepthStencilStateFactory( ) = default;
};

std::unique_ptr<IDepthStencilStateFactory> CreateDepthStencailStateFactory( );