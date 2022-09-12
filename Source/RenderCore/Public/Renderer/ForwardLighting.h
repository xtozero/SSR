#pragma once

#include "Buffer.h"
#include "GraphicsApiResource.h"
#include "Math/Vector.h"
#include "Math/Vector4.h"
#include "SizedTypes.h"
#include "TypedBuffer.h"

namespace rendercore
{
	class ForwardLightBuffer
	{
	public:
		void Initialize( uint32 bytePerElement, uint32 numElements, RESOURCE_FORMAT format );

		void* Lock();
		void Unlock();

		aga::ShaderResourceView* SRV();
		const aga::ShaderResourceView* SRV() const;

		aga::Buffer* Resource();
		const aga::Buffer* Resource() const;

	private:
		RefHandle<aga::Buffer> m_buffer;
	};

	struct ForwardLightData
	{
		Vector4 m_positionAndRange;
		ColorF m_diffuse;
		ColorF m_specular;
		Vector4 m_attenuationAndFalloff;
		Vector4 m_directionAndType;
		Vector4 m_spotAngles;
	};

	struct ForwardLightConstant
	{
		uint32 m_numLight = 0;
		Vector m_cameraPos;
	};

	struct ForwardLightingResource
	{
		TypedConstatBuffer<ForwardLightConstant> m_lightConstant;
		ForwardLightBuffer m_lightBuffer;
	};
}
