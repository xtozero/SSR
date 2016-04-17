#pragma once

#include "Light.h"
#include "../Engine/ScriptKeyHandler.h"

#include <array>
#include <memory>
#include <vector>

namespace
{
	const int MAX_LIGHTS = 180;
}

struct ShaderLightTrait
{
	int m_curLights;
	D3DXVECTOR3 m_cameraPos;
	D3DXCOLOR m_globalAmbient;
	std::array<LightTrait, MAX_LIGHTS> m_properties;

	ShaderLightTrait() :
		m_curLights( 0 ),
		m_cameraPos( 0.f, 0.f, 0.f ),
		m_globalAmbient{ 0.f, 0.f, 0.f, 1.0f }
	{}
};

class KeyValueGroup;
class CCamera;

class CLightManager : public CScriptKeyHandler<CLightManager>
{
public:
	bool Initialize( std::vector<std::shared_ptr<CGameObject>>& objectList );
	void UpdateToRenderer( const CCamera& camera );

	void SetCameraPosition( const D3DXVECTOR3& cameraPos );
	void SetGlobalAmbient( const D3DXCOLOR& globalAmbient );

	void PushLightTrait( const LightTrait& trait );

	CLightManager( );
	~CLightManager( ) = default;

private:
	void LoadPropertyFromScript( );
	void LoadLightProperty( const std::shared_ptr<KeyValueGroup>& pKeyValues );

	std::array<std::shared_ptr<CLight>, MAX_LIGHTS > m_lights;
	ShaderLightTrait m_shaderLightProperty;

	bool m_needUpdateToRenderer;
};