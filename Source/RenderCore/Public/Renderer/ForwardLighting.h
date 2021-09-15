#pragma once

#include "Buffer.h"
#include "GraphicsApiResource.h"
#include "Math/CXMFloat.h"
#include "SizedTypes.h"
#include "TypedBuffer.h"

class ForwardLightBuffer
{
public:
	void Initialize( uint32 bytePerElement, uint32 numElements, RESOURCE_FORMAT format );

	void* Lock( );
	void Unlock( );

	aga::ShaderResourceView* SRV( );
	const aga::ShaderResourceView* SRV( ) const;

	aga::Buffer* Resource( );
	const aga::Buffer* Resource( ) const;

private:
	RefHandle<aga::Buffer> m_buffer;
};

struct ForwardLightData
{
	CXMFLOAT4 m_positionAndRange;
	CXMFLOAT4 m_diffuse;
	CXMFLOAT4 m_specular;
	CXMFLOAT4 m_attenuationAndFalloff;
	CXMFLOAT4 m_directionAndType;
	CXMFLOAT4 m_spotAngles;
};

struct ForwardLightConstant
{
	uint32 m_numLight = 0;
	CXMFLOAT3 m_cameraPos;
};

struct ForwardLightingResource
{
	TypedConstatBuffer<ForwardLightConstant> m_lightConstant;
	ForwardLightBuffer m_lightBuffer;
};