#pragma once

#include "Buffer.h"
#include "GraphicsApiResource.h"
#include "Math/Vector.h"
#include "Math/Vector4.h"
#include "SizedTypes.h"
#include "ShaderArguments.h"

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
		RefHandle<agl::Buffer> m_buffer;
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

	BEGIN_SHADER_ARGUMENTS_STRUCT( ForwardLightParameters )
		DECLARE_VALUE( uint32, NumLight )
		DECLARE_VALUE( Vector, HemisphereLightUpVector )
		DECLARE_VALUE( ColorF, HemisphereLightUpperColor )
		DECLARE_VALUE( ColorF, HemisphereLightLowerColor )
		DECLARE_ARRAY_VALUE( Vector4, IrradianceMapSH, 7 )
		DECLARE_VALUE( float, ReflectionMipLevels )
	END_SHADER_ARGUMENTS_STRUCT()

	struct ForwardLightingResource final
	{
		RefHandle<ShaderArguments> m_shaderArguments;
		ForwardLightBuffer m_lightBuffer;

		ForwardLightingResource();
	};
}
