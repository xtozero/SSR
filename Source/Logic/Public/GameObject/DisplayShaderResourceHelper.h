#pragma once

#include "GameObject.h"

class CDisplayShaderResourceHelper : public CGameObject
{
public:
	virtual void SetPosition( const CXMFLOAT3& pos ) override;
	virtual void Render( CGameLogic& gameLogic ) override;
	virtual void Think( float elapsedTime ) override;

	virtual void LoadProperty( const JSON::Value& json ) override;

	virtual bool IgnorePicking( ) const override { return ShouldDraw( ); }

	virtual bool ShouldDraw( ) const override;

protected:
	virtual bool LoadModelMesh( CGameLogic& gameLogic ) override;

private:
	float m_width;
	float m_height;
	
	std::string m_textureName;

public:
	CDisplayShaderResourceHelper( );
	~CDisplayShaderResourceHelper( ) = default;
};


