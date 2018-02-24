#pragma once

#include "common.h"
#include "INotifyGraphicsDevice.h"
#include "Light.h"

#include "../RenderCore/CommonRenderer/Resource.h"
#include "../shared/Math/CXMFloat.h"

#include <array>
#include <memory>
#include <vector>

namespace
{
	constexpr int MAX_LIGHTS = 180;
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
class KeyValue;

class CLightManager : IGraphicsDeviceNotify
{
public:
	virtual void OnDeviceRestore( CGameLogic& gameLogic ) override;

	bool Initialize( IRenderer& renderer, std::vector<std::unique_ptr<CGameObject>>& objectList );
	void UpdateToRenderer( IRenderer& renderer, const CCamera& camera );

	void SetCameraPosition( const CXMFLOAT3& cameraPos );
	void SetGlobalAmbient( const CXMFLOAT4& globalAmbient );

	void PushLightTrait( const LightTrait& trait );

	CXMFLOAT4X4 GetPrimaryLightViewMatrix( );
	CXMFLOAT4X4 GerPrimaryLightProjectionMatrix( );

	CLightManager( );
	~CLightManager( ) = default;

private:
	bool CreateDeviceDependentResource( IRenderer& renderer );
	void LoadPropertyFromScript( );
	void LoadLightProperty( const KeyValue& keyValue );

	std::array<CLight*, MAX_LIGHTS > m_lights;
	ShaderLightTrait m_shaderLightProperty;

	bool m_needUpdateToRenderer;

	UINT m_primaryLight;

	RE_HANDLE m_lightBuffer = RE_HANDLE_TYPE::INVALID_HANDLE;
};