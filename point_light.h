#pragma once

#include "bgfx_entity.h"

namespace bgfx
{

class PointLight : public BGFXEntity
{

public:
	glm::vec3 _color;
	float _intensity;
};

}