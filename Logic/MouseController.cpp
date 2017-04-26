#include "stdafx.h"
#include "common.h"
#include "IListener.h"
#include "MouseController.h"

#include "../shared/UserInput.h"
#include "../shared/Util.h"

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
