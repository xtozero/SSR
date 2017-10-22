#pragma once

#include "GameObject.h"

class KeyValue;

class CDisplayShaderResourceHelper : public CGameObject
{
public:
	virtual void SetPosition( const CXMFLOAT3& pos ) override;
	virtual void Render( CGameLogic& gameLogic ) override;
	virtual void Think( ) override;

	virtual void LoadPropertyFromScript( const KeyValue& pKeyValue ) override;

	virtual bool IgnorePicking( ) const override { return ShouldDraw( ); }

	virtual bool ShouldDraw( ) const override;
protected:
	virtual bool LoadModelMesh( CGameLogic& gameLogic ) override;

private:

	float m_width;
	float m_height;
	String m_textureName;

public:
	CDisplayShaderResourceHelper( );
	~CDisplayShaderResourceHelper( ) = default;
};


