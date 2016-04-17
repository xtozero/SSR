#pragma once

#include "GameObject.h"

#include <array>

enum class LIGHT_TYPE
{
	NONE = -1,
	AMBIENT_LIGHT,
	DIRECTINAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT
};

struct LightTrait
{
	LIGHT_TYPE				m_type;
	bool					m_isOn;
	float					m_theta;
	float					m_phi;
	D3DXVECTOR3				m_direction;
	float					m_range;
	float					m_fallOff;
	D3DXVECTOR3				m_attenuation;
	D3DXVECTOR4				m_position;
	D3DXCOLOR				m_diffuse;
	D3DXCOLOR				m_specular;

	LightTrait( ) :
		m_type( LIGHT_TYPE::NONE ),
		m_isOn( false ),
		m_theta( 0.f ),
		m_phi( 0.f ),
		m_direction{ 0.f, 0.f, 0.f },
		m_range( 0.f ),
		m_fallOff( 0.f ),
		m_attenuation{ 0.f, 0.f, 0.f },
		m_diffuse{ 0.f, 0.f, 0.f, 0.f },
		m_specular{ 0.f, 0.f, 0.f, 0.f }
	{}
};

class CLight : public CGameObject
{
public:
	virtual void SetPosition( const float x, const float y, const float z ) override;
	virtual void SetPosition( const D3DXVECTOR3& pos ) override;
	virtual void SetScale( const float xScale, const float yScale, const float zScale ) override;
	virtual void SetRotate( const float pitch, const float yaw, const float roll ) override;
	
	virtual void Render( ) override;

	virtual bool ShouldDraw( ) const override { return false; }

	const LIGHT_TYPE GetType( ) const;
	const bool IsOn( ) const;
	D3DXVECTOR3 GetDirection( ) const;

	void SetOnOff( const bool on );
	void SetRange( const float range );
	void SetFallOff( const float fallOff );
	void SetConeProperty( const float theta, const float phi );
	void SetDiection( const D3DXVECTOR3& direction );
	void SetAttenuation( const D3DXVECTOR3& attenuation );
	void SetDiffuseColor( const D3DXCOLOR& diffuseColor );
	void SetSpecularColor( const D3DXCOLOR& specularColor );

	void RegisterProperty( LightTrait* const property ) { m_property = property; }

	CLight( );
	virtual ~CLight( ) = default;

private:
	LightTrait* m_property;
};