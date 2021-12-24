#pragma once

#include "PassProcessor.h"

class ShadowDrawPassProcessor : public IPassProcessor
{
public:
	virtual std::optional<DrawSnapshot> Process( const PrimitiveSubMesh& subMesh ) override;
};