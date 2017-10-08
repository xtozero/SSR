#pragma once
#include <memory>

class IMesh;
class IMeshBuilder;
class IMaterial;
class IRenderer;
class IRenderResource;

class ScreenBlurManager
{
public:
	bool Init( IRenderer& renderer, IMeshBuilder& meshBuilder );
	void Process( IRenderer& renderer, IRenderResource& destSRV, IRenderResource& destRT ) const;

private:
	IMesh* m_pScreenRect = nullptr;
	IMaterial* m_pBlurMaterial[2] = { nullptr, nullptr };
	IRenderResource* m_pBlurRt = nullptr;
	IRenderResource* m_pBlurSrv = nullptr;
};

