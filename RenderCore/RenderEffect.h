#pragma once

#include <memory>
#include <vector>

struct ID3D11Device;
struct ID3D11DeviceContext;
class CTextureManager;
class CShaderResourceManager;
class CRenderTargetManager;
class CSnapshotManager;

struct RenderContext
{
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pDeviceContext;
	CTextureManager*		m_pTextureMgr;
	CShaderResourceManager*	m_pShaderResourceMgr;
	CRenderTargetManager*	m_pRenderTargetMgr;
	CSnapshotManager*		m_pSnapshotMgr;
};

class IRenderEffect
{
public:
	virtual void SceneBegin( const RenderContext& context ) = 0;
	virtual void SceneEnd( const RenderContext& context ) = 0;

	virtual ~IRenderEffect( ) {}
};

class CRenderEffect
{
public:
	bool Initialize( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext,
		CTextureManager* pTextureMgr,
		CShaderResourceManager* pShaderResourceMgr,
		CRenderTargetManager* pRenderTargetMgr,
		CSnapshotManager* pSnapshotMgr );

	void SceneBegin( );
	void SceneEnd( );

	CRenderEffect( );
private:
	std::vector<std::unique_ptr<IRenderEffect>> m_renderEffects;
	RenderContext m_context;
};
