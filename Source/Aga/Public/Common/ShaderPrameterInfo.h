#pragma once

#include "ShaderPrameterMap.h"

#include <vector>

class ShaderParameterInfo
{
public:
	std::vector<ShaderParameter> m_constantBuffers;
	std::vector<ShaderParameter> m_srvs;
	std::vector<ShaderParameter> m_uavs;
	std::vector<ShaderParameter> m_samplers;
};