#pragma once

#include "Buffer.h"
#include "GraphicsApiResource.h"
#include "Math/CXMFloat.h"
#include "TypedBuffer.h"

class ForwardLightBuffer
{
public:
	void Initialize( std::size_t bytePerElement, std::size_t numElements, RESOURCE_FORMAT format );

	void* Lock( );
	void Unlock( );

	aga::ShaderResourceView* SRV( );
	const aga::ShaderResourceView* SRV( ) const;

	aga::Buffer* Resource( );
	const aga::Buffer* Resource( ) const;

private:
	std::size_t m_size = 0;
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
	unsigned int m_numLight = 0;
	CXMFLOAT3 m_cameraPos;
};

struct ForwardLightingResource
{
	TypedConstatBuffer<ForwardLightConstant> m_lightConstant;
	ForwardLightBuffer m_lightBuffer;
};