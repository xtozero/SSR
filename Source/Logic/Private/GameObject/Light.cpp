#include "stdafx.h"
#include "GameObject/Light.h"

#include <cassert>

using namespace DirectX;

bool CLight::Initialize( CGameLogic& /*gameLogic*/, std::size_t id )
{
	SetID( id );
	return true;
}

void CLight::SetPosition( const float x, const float y, const float z )
{
	CGameObject::SetPosition( x, y, z );
	m_needRebuildTransform = false;

	if ( m_property )
	{
		m_property->m_position = CXMFLOAT4( x, y, z, 0.f );
	}
}

void CLight::SetPosition( const CXMFLOAT3& pos )
{
	SetPosition( pos.x, pos.y, pos.z );
}

void CLight::SetScale( const float /*xScale*/, const float /*yScale*/, const float /*zScale*/ )
{
	//Do Nothing
}

void CLight::SetRotate( const float /*pitch*/, const float /*yaw*/, const float /*roll*/ )
{
	//DO Nothing
}

void CLight::Render( CGameLogic& /*gameLogic*/ )
{
	//Do Nothing Yet
}

const LIGHT_TYPE CLight::GetType( ) const
{
	if ( m_property )
	{
		return m_property->m_type;
	}

	return LIGHT_TYPE::NONE;
}

const bool CLight::IsOn( ) const
{
	if ( m_property )
	{
		return m_property->m_isOn;
	}

	return false;
}

CXMFLOAT3 CLight::GetDirection( ) const
{
	if ( m_property )
	{
		return m_property->m_direction;
	}

	CXMFLOAT3 default = { 0.f, 0.f, 0.f };
	return default;
}

void CLight::SetOnOff( const bool on )
{
	if ( m_property )
	{
		m_property->m_isOn = on;
	}
}

void CLight::SetRange( const float range )
{
	if ( m_property )
	{
		m_property->m_range = range;
	}
}

void CLight::SetFallOff( const float fallOff )
{
	if ( m_property )
	{
		m_property->m_fallOff = fallOff;
	}
}

void CLight::SetConeProperty( const float theta, const float phi )
{
	if ( m_property )
	{
		m_property->m_theta = theta;
		m_property->m_phi = phi;
	}
}

void CLight::SetDiection( const CXMFLOAT3& direction )
{
	if ( m_property )
	{
		m_property->m_direction = XMVector3Normalize( direction );
	}
}

void CLight::SetAttenuation( const CXMFLOAT3& attenuation )
{
	if ( m_property )
	{
		m_property->m_attenuation = attenuation;
	}
}

void CLight::SetDiffuseColor( const CXMFLOAT4& diffuseColor )
{
	if ( m_property )
	{
		m_property->m_diffuse = diffuseColor;
	}
}

void CLight::SetSpecularColor( const CXMFLOAT4& specularColor )
{
	if ( m_property )
	{
		m_property->m_specular = specularColor;
	}
}

CXMFLOAT4X4 CLight::GetViewMatrix( )
{
	if ( m_isNeedReclac )
	{
		CXMFLOAT3 eyePos( m_property->m_position.x, m_property->m_position.y, m_property->m_position.z );
		assert( m_property->m_direction.x != 0.f || m_property->m_direction.y != 0.f || m_property->m_direction.z != 0.f );
		CXMFLOAT3 dir( m_property->m_direction.x, m_property->m_direction.y, m_property->m_direction.z );
		XMVECTOR lookAt = eyePos + dir;
		CXMFLOAT3 up( 0.f, 1.0f, 0.f );

		m_viewMatrix = XMMatrixLookAtLH( eyePos, lookAt, up );
		m_isNeedReclac = false;
	}

	return m_viewMatrix;
}

CLight::CLight( ) :
	m_property( nullptr ),
	m_isNeedReclac( true )
{
}