#pragma once

#include "BaseMesh.h"
#include "Material/Material.h"

#include <vector>

struct MeshDescription;
class StaticMeshRenderData;

class Material;

class StaticMesh : public BaseMesh
{
public:
	StaticMesh( MeshDescription&& meshDescription, std::vector<Material>&& materials );

private:
	StaticMeshRenderData* m_renderData = nullptr;
	std::vector<Material> m_materials;
};
