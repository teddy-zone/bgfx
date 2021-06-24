#include "camera.h"

#include "glm/gtc/matrix_transform.hpp"

namespace bgfx
{

Camera::Camera(float width, float height):
	_height(height),
	_width(width)
{
}

void Camera::set_position(const glm::vec3& new_pos)
{
	_position = new_pos;
}

void Camera::translate(const glm::vec3& trans)
{
	_position += trans;
}

void Camera::set_look_vector(const glm::vec3& new_look)
{
	_look = new_look;
}

void Camera::set_look_target(const glm::vec3& new_look_target)
{
	_look = glm::normalize(new_look_target - _position);
}

const glm::mat4& Camera::get_view_mat()
{
	_view_mat = glm::lookAt(_position, _position + _look, glm::vec3(0, 0, 1));
	return _view_mat;
}

const glm::mat4& Camera::get_projection_mat()
{
	_projection_mat = glm::perspective(glm::radians(_fov), _width / _height, 0.1f, 100.0f);
	return _projection_mat;
}

void Camera::draw_object(RenderableMesh& in_mesh)
{
	in_mesh.bind(get_view_mat(), get_projection_mat());
	in_mesh.draw();
}

}  // namespace bgfx