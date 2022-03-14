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

glm::vec3 Camera::get_position() const
{
	return _position;
}

void Camera::translate(const glm::vec3& trans)
{
	_position += trans;
}

glm::vec3 Camera::get_look() const
{
	return _look;
}

void Camera::set_look_vector(const glm::vec3& new_look)
{
	_look = new_look;
	calc_vectors();
}

void Camera::set_look_target(const glm::vec3& new_look_target)
{
	_look = glm::normalize(new_look_target - _position);
	calc_vectors();
}

const glm::mat4& Camera::get_view_mat()
{
	_view_mat = glm::lookAt(_position, _position + _look, glm::vec3(0, 0, 1));
	return _view_mat;
}

const glm::mat4& Camera::get_projection_mat()
{
	_projection_mat = glm::perspective(glm::radians(_fov), _width / _height, 0.1f, 2000.0f);
	return _projection_mat;
}

void Camera::draw_object(RenderableMesh& in_mesh, std::shared_ptr<bgfx::Material> in_mat)
{
	in_mesh.bind(get_view_mat(), get_projection_mat(), in_mat);
	in_mesh.draw();
}

void Camera::draw_object(LineObject& in_mesh)
{
	in_mesh.bind(get_view_mat(), get_projection_mat());
	in_mesh.draw();
}

glm::vec3 Camera::get_ray(float x, float y)
{
	glm::vec3 pixel_loc = glm::normalize(_look * _f + _right * (x - 0.5f) * 1.0f + _up * (y - 0.5f) * _height/_width);
	return pixel_loc;
}

void Camera::calc_vectors()
{
	_right = glm::normalize(glm::cross(_look, glm::vec3(0, 0, 1)));
	_up = glm::normalize(glm::cross(_right, _look));
	_f = (_height/_width) / tan(glm::radians(_fov) / 2.0) / 2.0;
}

}  // namespace bgfx