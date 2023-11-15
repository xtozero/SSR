#pragma once

#include "Buffer.h"
#include "GraphicsApiResource.h"
#include "Math/Vector.h"
#include "Math/Vector4.h"
#include "SizedTypes.h"
#include "TypedBuffer.h"

namespace rendercore
{
	class ForwardLightBuffer final
	{
	public:
		void Initialize( uint32 bytePerElement, uint32 numElements, agl::ResourceFormat format );

		void* Lock();
		void Unlock();

		agl::ShaderResourceView* SRV();
		const agl::ShaderResourceView* SRV() const;

		agl::Buffer* Resource();
		const agl::Buffer* Resource() const;

	private:
		agl::RefHandle<agl::Buffer> m_buffer;
	};

	struct ForwardLightData final
	{
		Vector4 m_positionAndRange;
		ColorF m_diffuse;
		ColorF m_specular;
		Vector4 m_attenuationAndFalloff;
		Vector4 m_directionAndType;
		Vector4 m_spotAngles;
	};

	struct ForwardLightConstant final
	{
		uint32 m_numLight = 0;
		Vector m_hemisphereLightUpVector;
		ColorF m_hemisphereLightUpperColor;
		ColorF m_hemisphereLightLowerColor;
		Vector4 m_irrdianceMapSH[7];
	};

	struct ForwardLightingResource final
	{
		TypedConstatBuffer<ForwardLightConstant> m_lightConstant;
		ForwardLightBuffer m_lightBuffer;
	};
}
