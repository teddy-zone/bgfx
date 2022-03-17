#pragma once

#include "bgfx_entity.h"

namespace bgfx
{

struct PointLight
{
	glm::vec4 location;
	glm::vec4 color;
	float intensity;
	float pad[3];
};

}