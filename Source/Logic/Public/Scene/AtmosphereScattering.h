#pragma once

#include "INotifyGraphicsDevice.h"
#include "Math/Vector.h"
#include "Math/Vector4.h"
//#include "Render/Resource.h"

class CGameLogic;
class IRenderer;

class CAtmosphericScatteringManager : public IGraphicsDeviceNotify
{
public:
	virtual void OnDeviceRestore( CGameLogic& gameLogic ) override;

	bool Init( CGameLogic& gameLogic );

	void Precompute( IRenderer& renderer );
	void Render( IRenderer& renderer, Point cameraPos, Vector sunDir );

private:
	bool CreateDeviceDependendResource( IRenderer& renderer );

	//// Transmittance
	//RE_HANDLE m_transmittanceTex = RE_HANDLE::InValidHandle( );
	//RE_HANDLE m_transmittanceSrv = RE_HANDLE::InValidHandle( );

	//// Irradiance
	//RE_HANDLE m_irradianceTex = RE_HANDLE::InValidHandle( );
	//RE_HANDLE m_irradianceSrv = RE_HANDLE::InValidHandle( );

	//// Inscatter
	//RE_HANDLE m_inscatterTex = RE_HANDLE::InValidHandle( );
	//RE_HANDLE m_inscatterSrv = RE_HANDLE::InValidHandle( );

	// Constant Buffer
	struct AtmophericScatteringParameter
	{
		Vector4 m_cameraPos;
		Vector4 m_sunDir;
		float m_exposure;
		float m_padding[3];
	};
	//RE_HANDLE m_parameterBuf = RE_HANDLE::InValidHandle( );;

	//// Shader
	//RE_HANDLE m_atmosphericVs = RE_HANDLE::InValidHandle( );
	//RE_HANDLE m_atmosphericPs = RE_HANDLE::InValidHandle( );

	//// Depth state
	//RE_HANDLE m_atmosphericDepthState = RE_HANDLE::InValidHandle( );

	//// Sampler state
	//RE_HANDLE m_atmosphericSampler = RE_HANDLE::InValidHandle( );
};