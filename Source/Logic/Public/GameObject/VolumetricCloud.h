#pragma once

#include "GameObject.h"

namespace logic
{
	class VolumetricCloudComponent;

	class VolumetricCloud : public GameObject
	{
		GENERATE_CLASS_TYPE_INFO( VolumetricCloud )

	public:
		VolumetricCloud();

	private:
		VolumetricCloudComponent* m_volumetricCloudComponent = nullptr;
	};
}