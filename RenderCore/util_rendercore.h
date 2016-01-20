#pragma once

void SetDebugName( ID3D11DeviceChild* child, const std::string& name );
template<typename T>
int GetRefCount( T* comObject )
{
	if ( comObject )
	{
		comObject->AddRef( );
		return comObject->Release( );
	}

	return -1;
}