#include "stdafx.h"
#include "D3D11Buffer.h"

#include "D3D11Api.h"
#include "D3D11FlagConvertor.h"

namespace
{
	D3D11_BUFFER_DESC ConvertTraitToDesc( const BUFFER_TRAIT& trait )
	{
		UINT byteWidth = trait.m_count * trait.m_stride;
		D3D11_USAGE usage = ConvertAccessFlagToUsage( trait.m_access );
		UINT bindFlag = ConvertTypeToBind( trait.m_bindType );
		UINT cpuAccessFlag = ConvertAccessFlagToCpuFlag( trait.m_access );
		UINT miscFlags = ConvertMicsToDXMisc( trait.m_miscFlag );
		UINT structureByteStride = trait.m_stride;

		return D3D11_BUFFER_DESC{
			byteWidth,
			usage,
			bindFlag,
			cpuAccessFlag,
			miscFlags,
			structureByteStride
		};
	}
}

void CD3D11Buffer::InitResource( )
{
	bool result = SUCCEEDED( D3D11Device( ).CreateBuffer( &m_desc, ( m_initData.pSysMem == nullptr ) ? nullptr : &m_initData, m_pResource.GetAddressOf( ) ) );
	assert( result );
}

CD3D11Buffer::CD3D11Buffer( const BUFFER_TRAIT & trait, const RESOURCE_INIT_DATA* initData )
{
	if ( initData )
	{
		m_dataStorage = new unsigned char[initData->m_srcSize];

		m_initData.SysMemPitch = initData->m_pitch;
		m_initData.SysMemSlicePitch = initData->m_slicePitch;
	}
	m_initData.pSysMem = m_dataStorage;

	m_desc = ConvertTraitToDesc( trait );
}