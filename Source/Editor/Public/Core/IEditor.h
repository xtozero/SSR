#pragma once

#include "Core/ILogic.h"
#include "GuideTypes.h"

namespace editor
{
	class IEditor : public logic::ILogic
	{
	};

	Owner<IEditor*> CreateEditor();
	void DestroyEditor( Owner<IEditor*> pEditor );
}
