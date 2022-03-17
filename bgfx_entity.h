#pragma once

#include "glm/glm.hpp"

namespace bgfx
{

class BGFXEntity
{

	glm::vec3 _position;
	glm::mat4 _rotation;
	glm::vec3 _scale;
	glm::mat4 _transform;

	void update_transform();

public:

	BGFXEntity();

	glm::mat4& get_transform();

	void translate(const glm::vec3& translation);

	void rotate(const glm::mat4& rotation);

	void scale(const glm::vec3& scale);

	void set_position(const glm::vec3& new_pos);

	void set_scale(const glm::vec3& new_scale);

	void set_rotation(const glm::mat4& new_rotation);
};

}