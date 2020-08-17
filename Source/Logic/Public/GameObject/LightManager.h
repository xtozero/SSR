#pragma once

#include "common.h"
#include "Light.h"
#include "Math/CXMFloat.h"
//#include "Render/Resource.h"
#include "Scene/INotifyGraphicsDevice.h"

#include <array>
#include <memory>
#include <vector>

namespace
{
	constexpr std::size_t MAX_LIGHTS = 180;
}

struct ShaderLightTrait
{
	int m_curLights = 0;
	CXMFLOAT3 m_cameraPos;
	CXMFLOAT4 m_globalAmbient;
	std::array<LightTrait, MAX_LIGHTS> m_properties;
};

class CCamera;
class IRenderer;

class CLightManager : IGraphicsDeviceNotify
{
public:
	virtual void OnDeviceRestore( CGameLogic& gameLogic ) override;

	bool Initialize( CGameLogic& gameLogic );

	void UpdateToRenderer( IRenderer& renderer, const CCamera& camera );
	void OnLightPropertyUpdated( ) { m_needUpdateToRenderer = true; }

	void SetCameraPosition( const CXMFLOAT3& cameraPos );
	void SetGlobalAmbient( const CXMFLOAT4& globalAmbient );

	void PushLightTrait( const LightTrait& trait );

	CLight* GetPrimaryLight( ) { return m_lights[m_primaryLight]; }

	void LoadProperty( const char* lightAsset, size_t assertSize );

	CLightManager( );
	~CLightManager( ) = default;

private:
	bool CreateDeviceDependentResource( IRenderer& renderer );

	// for OrenNayar
	bool CreateOrenNayarLUTAndBind( IRenderer& renderer );

	std::array<CLight*, MAX_LIGHTS> m_lights;
	ShaderLightTrait m_shaderLightProperty;

	bool m_needUpdateToRenderer;

	UINT m_primaryLight;

	//RE_HANDLE m_lightBuffer = RE_HANDLE::InValidHandle( );
};