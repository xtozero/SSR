#pragma once

#include "INotifyGraphicsDevice.h"
#include "Math/CXMFloat.h"
#include "Render/Resource.h"

class CGameLogic;
class IRenderer;

class CAtmosphericScatteringManager : public IGraphicsDeviceNotify
{
public:
	virtual void OnDeviceRestore( CGameLogic& gameLogic ) override;

	bool Init( CGameLogic& gameLogic );

	void Precompute( IRenderer& renderer );
	void Render( IRenderer& renderer, CXMFLOAT3 cameraPos, CXMFLOAT3 sunDir );

private:
	bool CreateDeviceDependendResource( IRenderer& renderer );

	// Transmittance
	RE_HANDLE m_transmittanceTex = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_transmittanceSrv = RE_HANDLE_TYPE::INVALID_HANDLE;

	// Irradiance
	RE_HANDLE m_irradianceTex = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_irradianceSrv = RE_HANDLE_TYPE::INVALID_HANDLE;

	// Inscatter
	RE_HANDLE m_inscatterTex = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_inscatterSrv = RE_HANDLE_TYPE::INVALID_HANDLE;

	// Constant Buffer
	struct AtmophericScatteringParameter
	{
		CXMFLOAT4 m_cameraPos;
		CXMFLOAT4 m_sunDir;
		float m_exposure;
		float m_padding[3];
;	};
	RE_HANDLE m_parameterBuf = RE_HANDLE_TYPE::INVALID_HANDLE;;

	// Shader
	RE_HANDLE m_atmosphericVs = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_atmosphericPs = RE_HANDLE_TYPE::INVALID_HANDLE;

	// Depth state
	RE_HANDLE m_atmosphericDepthState = RE_HANDLE_TYPE::INVALID_HANDLE;

	// Sampler state
	RE_HANDLE m_atmosphericSampler = RE_HANDLE_TYPE::INVALID_HANDLE;
};