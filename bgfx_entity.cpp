#include "bgfx_entity.h"

#include "glm/gtc/matrix_transform.hpp"

namespace bgfx
{

BGFXEntity::BGFXEntity():
	_position(0, 0, 0),
	_rotation(glm::mat4(1)),
	_scale(1, 1, 1)
{
	update_transform();
}

void BGFXEntity::update_transform()
{
	_transform = glm::mat4(1);
	auto trans_mat = glm::translate(glm::mat4(1), _position);
	_transform = _transform * trans_mat;
	_transform = glm::scale(_transform, _scale);
	_transform = _transform * _rotation;
}

glm::mat4& BGFXEntity::get_transform()
{
	return _transform;
}

void BGFXEntity::translate(const glm::vec3& translation)
{
	_position += translation;
	_transform = glm::translate(_transform, translation);
}

void BGFXEntity::rotate(const glm::mat4& rotation)
{
	_rotation = _rotation * rotation;
	_transform = _transform * rotation;
}

void BGFXEntity::scale(const glm::vec3& scale)
{
	_scale *= scale;
	_transform = glm::scale(_transform, scale);
}

void BGFXEntity::set_position(const glm::vec3& new_pos)
{
	_position = new_pos;
	update_transform();
}

void BGFXEntity::set_scale(const glm::vec3& new_scale)
{
	_scale = new_scale;
	update_transform();
}

void BGFXEntity::set_rotation(const glm::mat4& new_rotation)
{
	_rotation = new_rotation;
	update_transform();
}

}  // namespace bgfx
