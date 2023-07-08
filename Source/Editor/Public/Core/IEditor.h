#pragma once

#include "Core/ILogic.h"
#include "GuideTypes.h"

class IEditor : public ILogic
{
};

Owner<IEditor*> CreateEditor();
void DestroyEditor( Owner<IEditor*> pEditor );