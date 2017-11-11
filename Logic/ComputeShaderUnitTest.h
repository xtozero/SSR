#pragma once

class CGameLogic;
class IBuffer;
class IComputeShader;
class IRenderResource;

class CComputeShaderUnitTest
{
public:
	void RunUnitTest( CGameLogic& gameLogic );

private:
	IComputeShader* m_pComputeShader = nullptr;
	IBuffer* m_pConstantBuffer = nullptr;
	IBuffer* m_pBuffer[2] = { nullptr, nullptr };
	IRenderResource* m_pSrv[2] = { nullptr, nullptr };
	IRenderResource* m_pUav[2] = { nullptr, nullptr };
	IBuffer* m_pReadBackBuffer = nullptr;
};

