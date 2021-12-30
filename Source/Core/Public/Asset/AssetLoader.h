#pragma once

#include "GuideTypes.h"
#include "IAssetLoader.h"

#include <memory>
#include <string>

Owner<IAssetLoader*> CreateAssetLoader( );
void DestoryAssetLoader( Owner<IAssetLoader*> pRenderOptionManager );