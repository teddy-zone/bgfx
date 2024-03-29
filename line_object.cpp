#include "line_object.h"

#include "glad/gl.h"
#include <GLFW/glfw3.h>

namespace bgfx
{

LineObject::LineObject():
	_position(0,0,0),
	_rotation(glm::mat4(1)),
	_scale(1,1,1)
{
	update_transform();
}

void LineObject::update_transform()
{
	_transform = glm::mat4(1);
	auto trans_mat = glm::translate(glm::mat4(1), _position);
	_transform = _transform * trans_mat;
	_transform = glm::scale(_transform, _scale);
	_transform = _transform * _rotation;
}

glm::mat4& LineObject::get_transform()
{
	return _transform;
}

void LineObject::translate(const glm::vec3& translation)
{
	_position += translation;
	_transform = glm::translate(_transform, translation);
}

void LineObject::rotate(const glm::mat4& rotation)
{
	_rotation = _rotation * rotation;
	_transform = _transform * rotation;
}

void LineObject::scale(const glm::vec3& scale)
{
	_scale *= scale;
	_transform = glm::scale(_transform, scale);
}

void LineObject::set_position(const glm::vec3& new_pos)
{
	_position = new_pos;
	update_transform();
}

void LineObject::set_scale(const glm::vec3& new_scale)
{
	_scale = new_scale;
	update_transform();
}

void LineObject::bind(const glm::mat4& view_mat, const glm::mat4& proj_mat, std::shared_ptr<bgfx::Material> in_mat)
{
	if (!in_mat)
	{
		in_mat = _material;
	}

#ifdef ENABLE_GRAPHICS
	in_mat->set_uniform_i1("object_id", 1);

	in_mat->use();
	
	in_mat->set_transform(_transform, "model_matrix");

	in_mat->set_transform(proj_mat, "projection_matrix");

	in_mat->set_transform(view_mat, "view_matrix");

	_mesh->bind();
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("GL ERROR!: %d", err);
		throw "opengl error";
	}
#endif 
}
/*
void LineObject::draw()
{
#ifdef ENABLE_GRAPHICS
	if (strip)
	{
		glDrawArrays(GL_LINE_STRIP, 0, _mesh->vertex_count());
	}
	else
	{
		glDrawArrays(GL_LINES, 0, _mesh->vertex_count());
	}

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("GL ERROR!: %d", err);
		throw "opengl error";
	}
#endif
}
*/

void LineObject::draw()
{
#ifdef ENABLE_GRAPHICS
	if (strip)
	{
		_mesh->_vertex_indices.bind(BindPoint::ELEMENT_ARRAY_BUFFER);
		if (_mesh->_instanced)
		{
			glDrawArraysInstanced(GL_LINE_STRIP, _mesh->_vertex_indices.get_size(), GL_UNSIGNED_INT,  _mesh->_instance_offsets.get_size());
		}
		else
		{
			glDrawArrays(GL_LINE_STRIP, 0, _mesh->vertex_count());
		}
	}
	else
	{
		_mesh->_vertex_indices.bind(BindPoint::ELEMENT_ARRAY_BUFFER);
		if (_mesh->_instanced)
		{
			glDrawArraysInstanced(GL_LINES, _mesh->_vertex_indices.get_size(), GL_UNSIGNED_INT, _mesh->_instance_offsets.get_size());
		}
		else
		{
			glDrawArrays(GL_LINES, 0, _mesh->vertex_count());
		}
	}
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("GL ERROR!: %d", err);
		throw "opengl error";
	}
#endif
}

void LineObject::set_material(std::shared_ptr<Material> in_mat)
{
	_material = in_mat;
}

void LineObject::set_mesh(std::shared_ptr<Mesh> in_mesh)
{
	_mesh = in_mesh;
}

std::shared_ptr<Mesh> LineObject::get_mesh()
{
	return _mesh;
}

}  // namespace bgfx
