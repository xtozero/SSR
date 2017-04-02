#pragma once
#include <memory>

class IMesh;
class IMeshBuilder;
class IMaterial;
class IRenderer;
class IRenderTarget;
class IShaderResource;

class ScreenBlurManager
{
public:
	bool Init( IRenderer& renderer, IMeshBuilder& meshBuilder );
	void Process( IRenderer& renderer, IShaderResource& destSRV, IRenderTarget& destRT ) const;

private:
	std::shared_ptr<IMesh> m_pScreenRect;
	IMaterial* m_pBlurMaterial[2] = { nullptr, nullptr };
	IRenderTarget* m_pBlurRt = nullptr;
	IShaderResource* m_pBlurSrv = nullptr;
};

