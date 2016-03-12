#pragma once

class CGameObject;

class CSkyBox : public CGameObject
{
public:
	virtual void Think( ) override;
	virtual void Render( ) override;

protected:
	virtual bool LoadModelMesh( ) override;

public:
	CSkyBox( );
	~CSkyBox( );
};
