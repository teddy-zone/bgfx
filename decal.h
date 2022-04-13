#pragma once

#include "bgfx_entity.h"

namespace bgfx
{

struct Decal 
{
	glm::vec4 location;
	glm::vec4 color;
	float radius;
    float intensity;
	float t;
	int type;
	int object_id;
	int pad2[2];
};

}