#pragma once

#include "glm/glm.hpp"
#include "renderable_mesh.h"

namespace bgfx
{

class Camera
{
	glm::vec3 _position;
	glm::vec3 _look;
	glm::mat4 _view_mat;
	glm::mat4 _projection_mat;
	float _fov = 60;
	float _width;
	float _height;

public:
	Camera(int width, int height);
	void set_position(const glm::vec3& new_pos);
	void translate(const glm::vec3& trans);
	void set_look_vector(const glm::vec3& new_look);
	void set_look_target(const glm::vec3& new_look_target);
	const glm::mat4& get_view_mat();
	const glm::mat4& get_projection_mat();
	void draw_object(RenderableMesh& in_mesh);
};

}  // namespace bgfx