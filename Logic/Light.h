#pragma once

#include "common.h"
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
	XMFLOAT3				m_direction;
	float					m_range;
	float					m_fallOff;
	XMFLOAT3				m_attenuation;
	XMFLOAT4				m_position;
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
	virtual void SetPosition( const XMFLOAT3& pos ) override;
	virtual void SetScale( const float xScale, const float yScale, const float zScale ) override;
	virtual void SetRotate( const float pitch, const float yaw, const float roll ) override;
	
	virtual void Render( IRenderer& renderer ) override;

	virtual bool ShouldDraw( ) const override { return false; }
	virtual bool ShouldDrawShadow( ) const override { return false; }

	const LIGHT_TYPE GetType( ) const;
	const bool IsOn( ) const;
	XMFLOAT3 GetDirection( ) const;

	void SetOnOff( const bool on );
	void SetRange( const float range );
	void SetFallOff( const float fallOff );
	void SetConeProperty( const float theta, const float phi );
	void SetDiection( const XMFLOAT3& direction );
	void SetAttenuation( const XMFLOAT3& attenuation );
	void SetDiffuseColor( const D3DXCOLOR& diffuseColor );
	void SetSpecularColor( const D3DXCOLOR& specularColor );

	void RegisterProperty( LightTrait* const property ) { m_property = property; }

	D3DXMATRIX GetViewMatrix( );

	CLight( );
	virtual ~CLight( ) = default;

private:
	LightTrait* m_property;
	bool m_isNeedReclac;
	D3DXMATRIX m_viewMatrix;
};