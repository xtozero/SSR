#include "stdafx.h"
#include "common.h"

void SetDebugName( ID3D11DeviceChild* child, const std::string& name )
{
	if ( child != nullptr && name.length( ) > 0 )
	{
		child->SetPrivateData( WKPDID_D3DDebugObjectName, name.size( ), name.c_str( ) );
	}
}