#pragma once

namespace rendercore
{
	class VolumetricFogProxy;

	class VolumetricFogSceneInfo
	{
	public:
		const VolumetricFogProxy* Proxy() const
		{
			return m_volumetricFogProxy;
		}

		VolumetricFogProxy*& Proxy()
		{
			return m_volumetricFogProxy;
		}

		void CreateRenderData();

		VolumetricFogSceneInfo( VolumetricFogProxy* proxy );

	private:
		VolumetricFogProxy* m_volumetricFogProxy = nullptr;
	};
}
