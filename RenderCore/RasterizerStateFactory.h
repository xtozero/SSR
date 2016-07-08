#pragma once

#include "common.h"
#include "../Engine/ScriptKeyHandler.h"

#include <memory>

struct D3D11_RASTERIZER_DESC;
struct ID3D11Device;
class IRenderState;

class IRasterizerStateFactory : public CScriptKeyHandler<IRasterizerStateFactory>
{
public:
	virtual void LoadDesc( ) = 0;
	virtual std::shared_ptr<IRenderState> GetRasterizerState( ID3D11Device* pDevice, const String& stateName ) = 0;
	virtual void AddRasterizerDesc( const String& descName, const D3D11_RASTERIZER_DESC& newDesc ) = 0;

	virtual ~IRasterizerStateFactory( ) = default;
};

std::unique_ptr<IRasterizerStateFactory> CreateRasterizerStateFactory( );