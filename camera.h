#pragma once

#include "glm/glm.hpp"
#include "renderable_mesh.h"
#include "line_object.h"

namespace bgfx
{

class Camera
{
	glm::vec3 _position;
	glm::vec3 _look;
	glm::mat4 _view_mat;
	glm::mat4 _projection_mat;
	glm::vec3 _right;
	glm::vec3 _up;
	float _f;

	void calc_vectors();

public:
	float _fov = 60;
	float _width;
	float _height;

	Camera(float width, float height);
	void set_position(const glm::vec3& new_pos);
	glm::vec3 get_position() const;
	void translate(const glm::vec3& trans);
	glm::vec3 get_look() const;
	void set_look_vector(const glm::vec3& new_look);
	void set_look_target(const glm::vec3& new_look_target);
	const glm::mat4& get_view_mat();
	const glm::mat4& get_projection_mat();
	void draw_object(RenderableMesh& in_mesh);
	void draw_object(LineObject& in_mesh);
	glm::vec3 get_ray(float x, float y);

};

}  // namespace bgfx