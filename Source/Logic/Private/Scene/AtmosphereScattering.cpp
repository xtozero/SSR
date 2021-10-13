#include "stdafx.h"
#include "Scene/AtmosphereScattering.h"

#include "AtmoshpericScattering/AtmosphereConstant.h"
#include "Core/GameLogic.h"
//#include "Render/IRenderer.h"
//#include "Render/IRenderResourceManager.h"
#include "SizedTypes.h"

using namespace DirectX;

void CAtmosphericScatteringManager::OnDeviceRestore( CGameLogic& gameLogic )
{
	Init( gameLogic );
}

bool CAtmosphericScatteringManager::Init( CGameLogic& gameLogic )
{
	//IRenderer& renderer = gameLogic.GetRenderer( );

	//if ( CreateDeviceDependendResource( renderer ) )
	//{
	//	Precompute( renderer );
	//	return true;
	//}
	//else
	//{
	//	return false;
	//}

	return true;
}

void CAtmosphericScatteringManager::Precompute( IRenderer& renderer )
{
	//IResourceManager& resourceMgr = renderer.GetResourceManager( );

	//RE_HANDLE precomputeSampler[] = { m_atmosphericSampler, m_atmosphericSampler, m_atmosphericSampler, m_atmosphericSampler, m_atmosphericSampler };
	//renderer.BindSamplerState( SHADER_TYPE::CS, 0, 5, precomputeSampler );

	//// 1. Transmittance Table
	//RE_HANDLE csTransmittance = renderer.CreateComputeShader( "./Shaders/csTransmittance.cso", nullptr );
	//if ( csTransmittance == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//RE_HANDLE transmittanceRav = resourceMgr.CreateTextureRandomAccess( m_transmittanceTex, "TransmittanceTable" );
	//if ( transmittanceRav == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//renderer.BindShader( SHADER_TYPE::CS, csTransmittance );
	//renderer.BindRandomAccessResource( 0, 1, &transmittanceRav );

	//renderer.Dispatch( TRANSMITTANCE_GROUP_X, TRANSMITTANCE_GROUP_Y );

	//resourceMgr.FreeResource( csTransmittance );
	//resourceMgr.FreeResource( transmittanceRav );

	//// 2. Ground irradiance due to direct sunlight
	//RE_HANDLE csIrradiance1 = renderer.CreateComputeShader( "./Shaders/csIrradiance1.cso", nullptr );
	//if ( csIrradiance1 == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//TEXTURE_TRAIT irradianceTexTrait = {
	//	static_cast<uint32>( IRRADIANCE_W ),
	//	static_cast<uint32>( IRRADIANCE_H ),
	//	1U,
	//	1U,
	//	0U,
	//	1U,
	//	RESOURCE_FORMAT::R32G32B32A32_FLOAT,
	//	RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
	//	RESOURCE_BIND_TYPE::SHADER_RESOURCE | RESOURCE_BIND_TYPE::UNORDERED_ACCESS,
	//	0U
	//};

	//RE_HANDLE deltaETex = resourceMgr.CreateTexture2D( irradianceTexTrait, "DeltaETable" );
	//if ( deltaETex == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//RE_HANDLE deltaERav = resourceMgr.CreateTextureRandomAccess( deltaETex, "DeltaETable" );
	//if ( deltaERav == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//renderer.BindShader( SHADER_TYPE::CS, csIrradiance1 );

	//renderer.BindRandomAccessResource( 0, 1, &deltaERav );
	//renderer.BindShaderResource( SHADER_TYPE::CS, 0, 1, &m_transmittanceSrv );

	//renderer.Dispatch( IRRADIANCE_GROUP_X, IRRADIANCE_GROUP_Y );

	//resourceMgr.FreeResource( csIrradiance1 );

	//// 3. Compute single scattering texture deltaS
	//RE_HANDLE csInscatter1 = renderer.CreateComputeShader( "./Shaders/csInscatter1.cso", nullptr );
	//if ( csInscatter1 == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//TEXTURE_TRAIT inscatterTexTrait = {
	//	static_cast<uint32>( RES_MU_S * RES_NU ),
	//	static_cast<uint32>( RES_MU ),
	//	static_cast<uint32>( RES_R ),
	//	1U,
	//	0U,
	//	1U,
	//	RESOURCE_FORMAT::R32G32B32A32_FLOAT,
	//	RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
	//	RESOURCE_BIND_TYPE::SHADER_RESOURCE | RESOURCE_BIND_TYPE::UNORDERED_ACCESS,
	//};

	//RE_HANDLE deltaSRTex = resourceMgr.CreateTexture3D( inscatterTexTrait, "DeltaSRTable" );
	//if ( deltaSRTex == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//RE_HANDLE deltaSRRav = resourceMgr.CreateTextureRandomAccess( deltaSRTex, "DeltaSRTable" );
	//if ( deltaSRRav == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//RE_HANDLE deltaSMTex = resourceMgr.CreateTexture3D( inscatterTexTrait, "DeltaSMTable" );
	//if ( deltaSMTex == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//RE_HANDLE deltaSMRav = resourceMgr.CreateTextureRandomAccess( deltaSMTex, "DeltaSMTable" );
	//if ( deltaSMRav == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//renderer.BindShader( SHADER_TYPE::CS, csInscatter1 );

	//RE_HANDLE deltaSRav[] = { deltaSRRav, deltaSMRav };
	//renderer.BindRandomAccessResource( 0, 2, deltaSRav );

	//renderer.Dispatch( INSCATTER1_GROUP_X, INSCATTER1_GROUP_Y, INSCATTER1_GROUP_Z );

	//resourceMgr.FreeResource( csInscatter1 );

	//// 4. Copy deltaS into inscatter texture S
	//RE_HANDLE csCopyInscatter = renderer.CreateComputeShader( "./Shaders/csCopyInscatter1.cso", nullptr );
	//if ( csCopyInscatter == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//RE_HANDLE deltaSRSrv = resourceMgr.CreateTextureShaderResource( deltaSRTex, "DeltaSRTable" );
	//if ( deltaSRSrv == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//RE_HANDLE deltaSMSrv = resourceMgr.CreateTextureShaderResource( deltaSMTex, "DeltaSMTable" );
	//if ( deltaSMSrv == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//// Inscatter
	//BUFFER_TRAIT inscatterBufTrait = {
	//	sizeof( float ) * 4,
	//	static_cast<uint32>( RES_MU_S * RES_NU * RES_MU * RES_R ),
	//	RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
	//	RESOURCE_BIND_TYPE::UNORDERED_ACCESS,
	//	RESOURCE_MISC::BUFFER_STRUCTURED,
	//	nullptr,
	//	0U,
	//	0U
	//};

	//RE_HANDLE inscatterBuf = resourceMgr.CreateBuffer( inscatterBufTrait );
	//if ( inscatterBuf == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//RE_HANDLE inscatterRav = resourceMgr.CreateBufferRandomAccess( inscatterBuf, "InscatterTable" );
	//if ( inscatterRav == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//renderer.BindShader( SHADER_TYPE::CS, csCopyInscatter );

	//RE_HANDLE inscatter[] = { inscatterRav, RE_HANDLE::InValidHandle( ) };
	//renderer.BindRandomAccessResource( 0, 2, inscatter );

	//RE_HANDLE deltaSSrv[] = { deltaSRSrv, deltaSMSrv };
	//renderer.BindShaderResource( SHADER_TYPE::CS, 2, 2, deltaSSrv );

	//renderer.Dispatch( INSCATTER1_GROUP_X, INSCATTER1_GROUP_Y, INSCATTER1_GROUP_Z );

	//resourceMgr.FreeResource( csCopyInscatter );

	//BUFFER_TRAIT precomputeBufTrait = {
	//	sizeof( uint32 ) * 4,
	//	1U,
	//	RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE ,
	//	RESOURCE_BIND_TYPE::CONSTANT_BUFFER,
	//	0U,
	//	nullptr,
	//	0U,
	//	0U
	//};

	//RE_HANDLE precomputeBuf = resourceMgr.CreateBuffer( precomputeBufTrait );
	//if ( precomputeBuf == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//RE_HANDLE csInscatterS = renderer.CreateComputeShader( "./Shaders/csInscatterS.cso", nullptr );
	//if ( csInscatterS == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//RE_HANDLE csIrradianceN = renderer.CreateComputeShader( "./Shaders/csIrradianceN.cso", nullptr );
	//if ( csIrradianceN == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//RE_HANDLE csInscatterN = renderer.CreateComputeShader( "./Shaders/csInscatterN.cso", nullptr );
	//if ( csInscatterN == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//RE_HANDLE csCopyIrradiance = renderer.CreateComputeShader( "./Shaders/csCopyIrradiance.cso", nullptr );
	//if ( csCopyIrradiance == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//RE_HANDLE csCopyInscatterN = renderer.CreateComputeShader( "./Shaders/csCopyInscatterN.cso", nullptr );
	//if ( csCopyInscatterN == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//RE_HANDLE deltaESrv = resourceMgr.CreateTextureShaderResource( deltaETex, "DeltaETable" );
	//if ( deltaESrv == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//RE_HANDLE deltaJTex = resourceMgr.CreateTexture3D( inscatterTexTrait, "DeltaJTable" );
	//if ( deltaJTex == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//RE_HANDLE deltaJRav = resourceMgr.CreateTextureRandomAccess( deltaJTex, "DeltaJTable" );
	//if ( deltaJRav == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//RE_HANDLE deltaJSrv = resourceMgr.CreateTextureShaderResource( deltaJTex, "DeltaJTable" );
	//if ( deltaJSrv == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//// Irradiance
	//float irradianceData[IRRADIANCE_W * IRRADIANCE_H * 4] = {};
	//BUFFER_TRAIT irradianceBufTrait = {
	//	sizeof( float ) * 4,
	//	static_cast<uint32>( IRRADIANCE_W * IRRADIANCE_H ),
	//	RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
	//	RESOURCE_BIND_TYPE::UNORDERED_ACCESS,
	//	RESOURCE_MISC::BUFFER_STRUCTURED,
	//	irradianceData,
	//	IRRADIANCE_W * 4,
	//	IRRADIANCE_W * IRRADIANCE_H * 4
	//};

	//RE_HANDLE irradianceBuf = resourceMgr.CreateBuffer( irradianceBufTrait );
	//if ( irradianceBuf == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//RE_HANDLE irradianceRav = resourceMgr.CreateBufferRandomAccess( irradianceBuf, "IrradianceTable" );
	//if ( irradianceRav == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//// loop for each scattering order
	//for ( uint32 order = 2; order <= 4; ++order )
	//{
	//	uint32* gpuScatteringOrder = static_cast<uint32*>( renderer.LockBuffer( precomputeBuf ) );
	//	gpuScatteringOrder[0] = order;
	//	renderer.UnLockBuffer( precomputeBuf );

	//	renderer.BindConstantBuffer( SHADER_TYPE::CS, 0, 1, &precomputeBuf );

	//	// Compute deltaJ
	//	renderer.BindShader( SHADER_TYPE::CS, csInscatterS );
	//	renderer.BindShaderResource( SHADER_TYPE::CS, 1, 1, &deltaESrv );
	//	renderer.BindRandomAccessResource( 0, 1, &deltaJRav );

	//	for ( uint32 i = 0; i < INSCATTERS_GROUP_Z; ++i )
	//	{
	//		gpuScatteringOrder = static_cast<uint32*>( renderer.LockBuffer( precomputeBuf ) );
	//		gpuScatteringOrder[0] = order;
	//		gpuScatteringOrder[1] = i;
	//		renderer.UnLockBuffer( precomputeBuf );

	//		renderer.BindConstantBuffer( SHADER_TYPE::CS, 0, 1, &precomputeBuf );

	//		renderer.Dispatch( INSCATTERS_GROUP_X, INSCATTERS_GROUP_Y );
	//		renderer.WaitGPU( );
	//	}

	//	// Compute deltaE
	//	renderer.BindShader( SHADER_TYPE::CS, csIrradianceN );
	//	renderer.BindShaderResource( SHADER_TYPE::CS, 1, 1, nullptr );
	//	renderer.BindRandomAccessResource( 0, 1, &deltaERav );

	//	renderer.Dispatch( IRRADIANCE_GROUP_X, IRRADIANCE_GROUP_Y );

	//	// Compute deltaS
	//	renderer.BindShader( SHADER_TYPE::CS, csInscatterN );
	//	renderer.BindShaderResource( SHADER_TYPE::CS, 4, 1, &deltaJSrv );
	//	renderer.BindShaderResource( SHADER_TYPE::CS, 2, 1, nullptr );
	//	renderer.BindRandomAccessResource( 0, 1, &deltaSRRav );

	//	renderer.Dispatch( INSCATTERN_GROUP_X, INSCATTERN_GROUP_Y, INSCATTERN_GROUP_Z );

	//	// Add deltaE into irradiance textrue E
	//	renderer.BindShader( SHADER_TYPE::CS, csCopyIrradiance );
	//	renderer.BindShaderResource( SHADER_TYPE::CS, 1, 1, &deltaESrv );
	//	renderer.BindRandomAccessResource( 0, 1, &irradianceRav );

	//	renderer.Dispatch( IRRADIANCE_GROUP_X, IRRADIANCE_GROUP_Y );

	//	// Add deltaS info inscatter texture S
	//	renderer.BindShader( SHADER_TYPE::CS, csCopyInscatterN );
	//	renderer.BindShaderResource( SHADER_TYPE::CS, 2, 1, &deltaSRSrv );
	//	renderer.BindRandomAccessResource( 0, 1, &inscatterRav );

	//	renderer.Dispatch( INSCATTERN_GROUP_X, INSCATTERN_GROUP_Y, INSCATTERN_GROUP_Z );

	//	renderer.BindShaderResource( SHADER_TYPE::CS, 4, 1, nullptr );
	//}

	//resourceMgr.FreeResource( deltaETex );
	//resourceMgr.FreeResource( deltaERav );
	//resourceMgr.FreeResource( deltaESrv );

	//resourceMgr.FreeResource( deltaSRTex );
	//resourceMgr.FreeResource( deltaSRRav );
	//resourceMgr.FreeResource( deltaSRSrv );

	//resourceMgr.FreeResource( deltaSMTex );
	//resourceMgr.FreeResource( deltaSMRav );
	//resourceMgr.FreeResource( deltaSMSrv );

	//resourceMgr.FreeResource( deltaJTex );
	//resourceMgr.FreeResource( deltaJRav );
	//resourceMgr.FreeResource( deltaJSrv );

	//resourceMgr.FreeResource( precomputeBuf );
	//resourceMgr.FreeResource( csInscatterS );
	//resourceMgr.FreeResource( csIrradianceN );
	//resourceMgr.FreeResource( csInscatterN );
	//resourceMgr.FreeResource( csCopyIrradiance );
	//resourceMgr.FreeResource( csCopyInscatterN );

	//// create intermediate buffer
	//BUFFER_TRAIT intermediateBufTrait = {
	//	sizeof( float ) * 4,
	//	static_cast<uint32>( RES_MU_S * RES_NU * RES_MU * RES_R ),
	//	RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE | RESOURCE_ACCESS_FLAG::CPU_READ,
	//	0U,
	//	0U,
	//	nullptr,
	//	0U,
	//	0U
	//};

	//RE_HANDLE intermediateBuf = renderer.CreateBuffer( intermediateBufTrait );
	//if ( intermediateBuf == RE_HANDLE::InValidHandle( ) )
	//{
	//	__debugbreak( );
	//}

	//// transfer irradiance buffer to texture
	//RESOURCE_REGION srcRegion = { 0, 0, static_cast<uint32>( IRRADIANCE_W * IRRADIANCE_H * sizeof( float ) * 4 ), 0, 1, 0, 1 };
	//RESOURCE_REGION destRegion = { 0, 0, static_cast<uint32>( IRRADIANCE_W * IRRADIANCE_H * sizeof(float) * 4 ), 0, 1, 0, 1 };
	//resourceMgr.CopyResource( intermediateBuf, &destRegion, irradianceBuf, &srcRegion );

	//float* data = static_cast<float*>( renderer.LockBuffer( intermediateBuf, BUFFER_LOCKFLAG::READ ) );
	//resourceMgr.UpdateResourceFromMemory( m_irradianceTex, data, sizeof( float ) * 4 * IRRADIANCE_W, sizeof( float ) * 4 * IRRADIANCE_W * IRRADIANCE_H );

	//renderer.UnLockBuffer( intermediateBuf );

	//// transfer inscatter buffer to texture
	//srcRegion = { 0, 0, static_cast<uint32>( RES_MU_S * RES_NU * RES_MU * RES_R * sizeof( float ) * 4 ), 0, 1, 0, 1 };
	//destRegion = { 0, 0, static_cast<uint32>( RES_MU_S * RES_NU * RES_MU * RES_R * sizeof( float ) * 4 ), 0, 1, 0, 1 };
	//resourceMgr.CopyResource( intermediateBuf, &destRegion, inscatterBuf, &srcRegion );

	//data = static_cast<float*>( renderer.LockBuffer( intermediateBuf, BUFFER_LOCKFLAG::READ ) );
	//resourceMgr.UpdateResourceFromMemory( m_inscatterTex, data, sizeof( float ) * 4 * RES_MU_S * RES_NU, sizeof( float ) * 4 * RES_MU_S * RES_NU * RES_MU );

	//renderer.UnLockBuffer( intermediateBuf );

	//resourceMgr.FreeResource( intermediateBuf );
	//resourceMgr.FreeResource( irradianceBuf );
	//resourceMgr.FreeResource( irradianceRav );
	//resourceMgr.FreeResource( inscatterBuf );
	//resourceMgr.FreeResource( inscatterRav );

	//renderer.BindShader( SHADER_TYPE::CS, RE_HANDLE::InValidHandle( ) );
	//renderer.BindSamplerState( SHADER_TYPE::CS, 0, 5, nullptr );
	//renderer.BindShaderResource( SHADER_TYPE::CS, 0, 5, nullptr );
	//renderer.BindRandomAccessResource( 0, 1, nullptr );
}

void CAtmosphericScatteringManager::Render( IRenderer& renderer, CXMFLOAT3 cameraPos, CXMFLOAT3 sunDir )
{
	//renderer.BindShader( SHADER_TYPE::VS, m_atmosphericVs );
	//renderer.BindShader( SHADER_TYPE::PS, m_atmosphericPs );

	//AtmophericScatteringParameter* param = static_cast<AtmophericScatteringParameter*>( renderer.LockBuffer( m_parameterBuf ) );
	//param->m_cameraPos = CXMFLOAT4( cameraPos.x, cameraPos.y, cameraPos.z, 0.f );
	//param->m_sunDir = CXMFLOAT4( sunDir.x, sunDir.y, sunDir.z, 0.f );
	//param->m_exposure = 0.4f;
	//renderer.UnLockBuffer( m_parameterBuf );

	//renderer.BindConstantBuffer( SHADER_TYPE::PS, PS_CONSTANT_BUFFER::ATMOSPHERIC, 1, &m_parameterBuf );

	//RE_HANDLE samplers[] = { m_atmosphericSampler, m_atmosphericSampler, m_atmosphericSampler };
	//renderer.BindSamplerState( SHADER_TYPE::PS, 0, 3, samplers );

	//RE_HANDLE precomputed[] = { m_transmittanceSrv, m_irradianceSrv, m_inscatterSrv };
	//renderer.BindShaderResource( SHADER_TYPE::PS, 0, 3, precomputed );

	//renderer.BindVertexBuffer( nullptr, 0, 1, nullptr, nullptr );
	//
	//renderer.BindRasterizerState( RE_HANDLE::InValidHandle( ) );
	//renderer.BindDepthStencilState( m_atmosphericDepthState );

	//// draw!
	//renderer.Draw( RESOURCE_PRIMITIVE::TRIANGLELIST, 3 );

	//renderer.BindShaderResource( SHADER_TYPE::PS, 0, 3, nullptr );
}

bool CAtmosphericScatteringManager::CreateDeviceDependendResource( IRenderer& renderer )
{
	//IResourceManager& resourceMgr = renderer.GetResourceManager( );

	//// Transmittance
	//TEXTURE_TRAIT transmittanceTexTrait = {
	//	static_cast<uint32>( TRANSMITTANCE_W ),
	//	static_cast<uint32>( TRANSMITTANCE_H ),
	//	1U,
	//	1U,
	//	0U,
	//	1U,
	//	RESOURCE_FORMAT::R32G32B32A32_FLOAT,
	//	RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
	//	RESOURCE_BIND_TYPE::SHADER_RESOURCE | RESOURCE_BIND_TYPE::UNORDERED_ACCESS,
	//	0U
	//};

	//m_transmittanceTex = resourceMgr.CreateTexture2D( transmittanceTexTrait, "TransmittanceTable" );
	//if ( m_transmittanceTex == RE_HANDLE::InValidHandle( ) )
	//{
	//	return false;
	//}

	//m_transmittanceSrv = resourceMgr.CreateTextureShaderResource( m_transmittanceTex,  "TransmittanceTable" );
	//if ( m_transmittanceSrv == RE_HANDLE::InValidHandle( ) )
	//{
	//	return false;
	//}

	//// Irradiance
	//TEXTURE_TRAIT irradianceTexTrait = {
	//	static_cast<uint32>( IRRADIANCE_W ),
	//	static_cast<uint32>( IRRADIANCE_H ),
	//	1U,
	//	1U,
	//	0U,
	//	1U,
	//	RESOURCE_FORMAT::R32G32B32A32_FLOAT,
	//	RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
	//	RESOURCE_BIND_TYPE::SHADER_RESOURCE,
	//	0U
	//};

	//m_irradianceTex = resourceMgr.CreateTexture2D( irradianceTexTrait, "IrradianceTable" );
	//if ( m_irradianceTex == RE_HANDLE::InValidHandle( ) )
	//{
	//	return false;
	//}

	//m_irradianceSrv = resourceMgr.CreateTextureShaderResource( m_irradianceTex, "IrradianceTable" );
	//if ( m_irradianceSrv == RE_HANDLE::InValidHandle( ) )
	//{
	//	return false;
	//}

	//// Inscatter
	//TEXTURE_TRAIT inscatterTexTrait = {
	//	static_cast<uint32>( RES_MU_S * RES_NU ),
	//	static_cast<uint32>( RES_MU ),
	//	static_cast<uint32>( RES_R ),
	//	1U,
	//	0U,
	//	1U,
	//	RESOURCE_FORMAT::R32G32B32A32_FLOAT,
	//	RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::GPU_WRITE,
	//	RESOURCE_BIND_TYPE::SHADER_RESOURCE,
	//};

	//m_inscatterTex = resourceMgr.CreateTexture3D( inscatterTexTrait, "InscatterTable" );
	//if ( m_inscatterTex == RE_HANDLE::InValidHandle( ) )
	//{
	//	return false;
	//}

	//m_inscatterSrv = resourceMgr.CreateTextureShaderResource( m_inscatterTex, "InscatterTable" );
	//if ( m_inscatterSrv == RE_HANDLE::InValidHandle( ) )
	//{
	//	return false;
	//}

	//// Constant buffer
	//BUFFER_TRAIT parameterBufTrait = {
	//	sizeof( AtmophericScatteringParameter ),
	//	1U,
	//	RESOURCE_ACCESS_FLAG::GPU_READ | RESOURCE_ACCESS_FLAG::CPU_WRITE,
	//	RESOURCE_BIND_TYPE::CONSTANT_BUFFER,
	//	0U,
	//	nullptr,
	//	0U,
	//	0U
	//};

	//m_parameterBuf = resourceMgr.CreateBuffer( parameterBufTrait );
	//if ( m_parameterBuf == RE_HANDLE::InValidHandle( ) )
	//{
	//	return false;
	//}

	//// Vertex shader
	//m_atmosphericVs = renderer.CreateVertexShader( "./Shaders/vsDrawAtmosphere.cso", nullptr );
	//if ( m_atmosphericVs == RE_HANDLE::InValidHandle( ) )
	//{
	//	return false;
	//}

	//// Pixel shader
	//m_atmosphericPs = renderer.CreatePixelShader( "./Shaders/psDrawAtmosphere.cso", nullptr );
	//if ( m_atmosphericPs == RE_HANDLE::InValidHandle( ) )
	//{
	//	return false;
	//}

	//// Depth state
	//m_atmosphericDepthState = resourceMgr.CreateDepthStencilState( "depthWriteTestOff" );
	//if ( m_atmosphericDepthState == RE_HANDLE::InValidHandle( ) )
	//{
	//	return false;
	//}

	//// Sampler state
	//m_atmosphericSampler = resourceMgr.CreateSamplerState( "default" );
	//if ( m_atmosphericSampler == RE_HANDLE::InValidHandle( ) )
	//{
	//	return false;
	//}

	return true;
}
