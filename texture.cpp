#include "texture.h"

#include "glad/gl.h"
#include <GLFW/glfw3.h>

namespace bgfx
{

int Texture::_current_texture_slot = 0;
std::deque<int> Texture::_free_slots;

Texture::Texture(const std::string& name):
	_name(name)
{
	glGenTextures(1, &_gl_id);
	if (!_free_slots.size())
	{
		_texture_slot = _current_texture_slot;
		_current_texture_slot += 1;
	}
	else
	{
		_texture_slot = _free_slots.back();
		_free_slots.pop_back();
	}
}

Texture::~Texture()
{
	_free_slots.push_front(_texture_slot);
	glDeleteTextures(1, &_gl_id);
}

Texture::Texture(const std::string& name, unsigned int size_x, unsigned int size_y):
	_size_x(size_x),
	_size_y(size_y),
	_name(name)
{
	_texture_slot = _current_texture_slot;
	_current_texture_slot += 1;
	glGenTextures(1, &_gl_id);
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("GL ERROR!: %d", err);
	}
	bind();
	
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("GL ERROR!: %d", err);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, size_x, size_y, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("GL ERROR!: %d", err);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::bind()
{
	glActiveTexture(GL_TEXTURE0 + _texture_slot);
	glBindTexture(GL_TEXTURE_2D, _gl_id);
}

void Texture::load_data(unsigned char* data, int in_size_x, int in_size_y)
{
    bind();
    _size_x = in_size_x;
    _size_y = in_size_y;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _size_x, _size_y, 0, GL_RGBA, GL_FLOAT, data);
}

void Texture::to_render(int in_size_x, int in_size_y)
{
	bind();
	_size_x = in_size_x;
	_size_y = in_size_y;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, in_size_x, in_size_y, 0, GL_RGB32F, GL_FLOAT, NULL);
}

void Texture::to_depth(int in_size_x, int in_size_y)
{
	bind();
	_size_x = in_size_x;
	_size_y = in_size_y;
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, in_size_x, in_size_y, 0,
		GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
	);
}

const std::string& Texture::name()
{
	return _name;
}

void Texture::set_interpolation_mode(InterpMode interp_mode)
{
	GLenum gl_interp_enum = GL_LINEAR;

	switch (interp_mode)
	{
	case InterpMode::LINEAR:
		gl_interp_enum = GL_LINEAR;
		break;
	case InterpMode::NEAREST:
		gl_interp_enum = GL_NEAREST;
		break;
	default:
		break;
	}
	bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_interp_enum);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_interp_enum);

}

}  // namespace bgfx
