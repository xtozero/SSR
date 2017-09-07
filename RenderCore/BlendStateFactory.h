#pragma once

#include "common.h"
#include "../Engine/ScriptKeyHandler.h"

#include <memory>

struct CD3D_BLEND_DESC;
struct D3D11_BLEND_DESC;
struct ID3D11Device;
class IRenderState;

class IBlendStateFactory : public CScriptKeyHandler<IBlendStateFactory>
{
public:
	virtual void LoadDesc( ) = 0;
	virtual IRenderState* GetBlendState( ID3D11Device* pDevice, const String& stateName ) = 0;
	virtual void AddBlendDesc( const String& descName, const CD3D_BLEND_DESC& newDesc ) = 0;

	virtual ~IBlendStateFactory( ) = default;
};

std::unique_ptr<IBlendStateFactory> CreateBlendStateFactory( );