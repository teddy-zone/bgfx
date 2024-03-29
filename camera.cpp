#include "camera.h"

#include "glm/gtc/matrix_transform.hpp"

namespace bgfx
{

Camera::Camera(float width, float height, int x_res, int y_res):
	_height(height),
	_width(width),
	_x_res(x_res),
	_y_res(y_res),
	_look_offset(0)
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
	return glm::normalize(_look + _look_offset);
}

glm::vec3 Camera::get_unoffset_look() const
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

void Camera::set_look_offset(const glm::vec3& new_look_offset)
{
	_look_offset = new_look_offset;
	calc_vectors();
}

const glm::mat4& Camera::get_view_mat()
{
	_view_mat = glm::lookAt(_position, _position + _look + _look_offset, glm::vec3(0, 0, 1));
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

void Camera::draw_object(LineObject& in_mesh, std::shared_ptr<bgfx::Material> in_mat)
{
	in_mesh.bind(get_view_mat(), get_projection_mat(), in_mat);
	in_mesh.draw();
}

glm::vec3 Camera::get_ray(float x, float y)
{
	glm::vec3 pixel_loc = glm::normalize(get_look() * _f + _right * (x - 0.5f) * _width / _height + _up * (y - 0.5f) * 1.0f);
	return pixel_loc;
}

void Camera::calc_vectors()
{
	glm::vec3 intermediate_look = get_look();
	_right = glm::normalize(glm::cross(intermediate_look, glm::vec3(0, 0, 1)));
	_up = glm::normalize(glm::cross(_right, intermediate_look));
	_f = 0.5f / tan(glm::radians(_fov) / 2.0f);
}

glm::vec4 Camera::world_to_screen_space(const glm::vec3& world_space)
{
	glm::vec4 cam_space = get_projection_mat()*get_view_mat()*glm::vec4(world_space, 1.0f);
	cam_space = (cam_space/cam_space.w + 1.0f)/2.0f;
	cam_space.x = cam_space.x*_x_res;
	cam_space.y = cam_space.y*_y_res;
	return cam_space;
}

}  // namespace bgfx