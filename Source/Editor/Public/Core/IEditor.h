#pragma once

#include "Core/ILogic.h"
#include "GuideTypes.h"

namespace editor
{
	class PanelSharedContext;

	class IEditor : public logic::ILogic
	{
	public:
		virtual PanelSharedContext& GetPanelSharedCtx() = 0;
	};

	Owner<IEditor*> CreateEditor();
	void DestroyEditor( Owner<IEditor*> pEditor );
}
