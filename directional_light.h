#pragma once

#include "bgfx_entity.h"

namespace bgfx
{

	class DirectionalLight : public BGFXEntity
	{

	public:
		glm::vec3 _color;
		float _intensity;
		glm::vec3 _direction;
	};

}