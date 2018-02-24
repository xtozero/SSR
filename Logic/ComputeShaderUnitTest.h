#pragma once

#include "../RenderCore/CommonRenderer/Resource.h"

class CGameLogic;

class CComputeShaderUnitTest
{
public:
	void RunUnitTest( CGameLogic& gameLogic );

private:
	RE_HANDLE m_computeShader = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_constantBuffer = RE_HANDLE_TYPE::INVALID_HANDLE;
	RE_HANDLE m_buffers[2] = { RE_HANDLE_TYPE::INVALID_HANDLE, RE_HANDLE_TYPE::INVALID_HANDLE };
	RE_HANDLE m_srvs[2] = { RE_HANDLE_TYPE::INVALID_HANDLE, RE_HANDLE_TYPE::INVALID_HANDLE };
	RE_HANDLE m_uavs[2] = { RE_HANDLE_TYPE::INVALID_HANDLE, RE_HANDLE_TYPE::INVALID_HANDLE };
	RE_HANDLE m_readBackBuffers = RE_HANDLE_TYPE::INVALID_HANDLE;
};

