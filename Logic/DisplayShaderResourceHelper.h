#pragma once

#include "GameObject.h"

class CKeyValueIterator;

class CDisplayShaderResourceHelper : public CGameObject
{
public:
	virtual void SetPosition( const D3DXVECTOR3& pos ) override;
	virtual void Render( ) override;
	virtual void Think( ) override;

	virtual bool LoadPropertyFromScript( const CKeyValueIterator& pKeyValue ) override;

	virtual bool IgnorePicking( ) const override { return ShouldDraw( ); }

	virtual bool ShouldDraw( ) const override;
protected:
	virtual bool LoadModelMesh( ) override;

private:

	float m_width;
	float m_height;
	String m_textureName;

public:
	CDisplayShaderResourceHelper( );
	~CDisplayShaderResourceHelper( ) = default;
};


