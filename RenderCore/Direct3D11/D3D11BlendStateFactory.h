#pragma once

#include "D3D11RenderState.h"

#include "../../shared/Util.h"
#include "../common.h"

#include <array>
#include <d3d11.h>
#include <map>
#include <memory>

struct ID3D11Device;
class CD3D11BlendState;
class KeyValue;

class CD3D11BlendStateFactory
{
public:
	void LoadDesc( );
	Owner<CD3D11BlendState*> GetBlendState( ID3D11Device& device, const String& stateName );

private:
	void LoadRasterizerDesc( const KeyValue& keyValue );

	std::map<String, CD3D_BLEND_DESC> m_blendStateDesc;
};
