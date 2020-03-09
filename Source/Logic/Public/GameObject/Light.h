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
	LIGHT_TYPE				m_type = LIGHT_TYPE::NONE;
	bool					m_isOn = false;
	float					m_theta = 0.f;
	float					m_phi = 0.f;
	CXMFLOAT3				m_direction;
	float					m_range = 0.f;
	float					m_fallOff = 0.f;
	CXMFLOAT3				m_attenuation;
	CXMFLOAT4				m_position;
	CXMFLOAT4				m_diffuse;
	CXMFLOAT4				m_specular;
};

class CLight : public CGameObject
{
public:
	virtual bool Initialize( CGameLogic& gameLogic, std::size_t id ) override;
	virtual void SetPosition( const float x, const float y, const float z ) override;
	virtual void SetPosition( const CXMFLOAT3& pos ) override;
	virtual void SetScale( const float xScale, const float yScale, const float zScale ) override;
	virtual void SetRotate( const float pitch, const float yaw, const float roll ) override;
	
	virtual void Render( CGameLogic& gameLogic ) override;

	virtual bool ShouldDraw( ) const override { return false; }
	virtual bool ShouldDrawShadow( ) const override { return false; }

	const LIGHT_TYPE GetType( ) const;
	const bool IsOn( ) const;
	CXMFLOAT3 GetDirection( ) const;

	void SetOnOff( const bool on );
	void SetRange( const float range );
	void SetFallOff( const float fallOff );
	void SetConeProperty( const float theta, const float phi );
	void SetDiection( const CXMFLOAT3& direction );
	void SetAttenuation( const CXMFLOAT3& attenuation );
	void SetDiffuseColor( const CXMFLOAT4& diffuseColor );
	void SetSpecularColor( const CXMFLOAT4& specularColor );

	void RegisterProperty( LightTrait* const property ) { m_property = property; }

	CXMFLOAT4X4 GetViewMatrix( );

	CLight( );
	virtual ~CLight( ) = default;

private:
	LightTrait* m_property;
	bool m_isNeedReclac;
	CXMFLOAT4X4 m_viewMatrix;
};