#pragma once

#include "GameObject.h"

namespace logic
{
	class TexturedSkyComponent;

	class CSkyBox : public CGameObject
	{
		GENERATE_CLASS_TYPE_INFO( CSkyBox )

	public:
		virtual bool IgnorePicking() const override { return true; }

		CSkyBox();
		virtual ~CSkyBox() override = default;

	private:
		TexturedSkyComponent* m_pTexturedSkyComponent = nullptr;
	};
}
