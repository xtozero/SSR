#include "stdafx.h"
#include "AgaDelegator.h"
#include "Shader.h"

AgaDelegator g_agaDelegator;

void AgaDelegator::BootUp( IAga* pAga )
{
	m_aga = pAga;
	m_defaultConstants.BootUp( );
}

void AgaDelegator::Shutdown( )
{
	m_defaultConstants.Shutdown( );
}

void AgaDelegator::Dispatch( UINT x, UINT y, UINT z )
{
	ComputeShader empty;
	m_defaultConstants.Commit( empty );

	m_aga->Dispatch( x, y, z );

	BindShader( empty );
}

AgaDelegator& GetAgaDelegator( )
{
	return g_agaDelegator;
}
