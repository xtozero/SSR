#include "stdafx.h"
#include "Components/Component.h"

#include "GameObject/GameObject.h"

Component::Component( CGameObject* pOwner ) : m_pOwner( pOwner )
{
	pOwner->m_components.emplace_back( this );
}
