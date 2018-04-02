#include "stdafx.h"
#include "UserInput/MouseController.h"

#include "common.h"
#include "Core/IListener.h"
#include "UserInput/UserInput.h"
#include "Util.h"

void CUserInputBroadCaster::ProcessInput( const UserInput& input )
{
	for ( const auto& listener : m_listeners )
	{
		listener->ProcessInput( input );
	}
}

void CUserInputBroadCaster::AddListener( IListener* listener )
{
	if ( listener )
	{
		m_listeners.push_back( listener );
	}
}
