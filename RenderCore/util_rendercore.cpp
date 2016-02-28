#include "stdafx.h"
#include "Direct3D11.h"
#include "util_rendercore.h"

#include <D3D11.h>

IRenderer* g_pRenderer = CreateDirect3D11Renderer( );

void SetDebugName( ID3D11DeviceChild* child, const std::string& name )
{
	if ( child != nullptr && name.length( ) > 0 )
	{
		child->SetPrivateData( WKPDID_D3DDebugObjectName, name.size( ), name.c_str( ) );
	}
}