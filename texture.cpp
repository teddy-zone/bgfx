#include "texture.h"

#include "glad/gl.h"
#include <GLFW/glfw3.h>

namespace bgfx
{

int Texture::_current_texture_slot;
std::deque<int> Texture::_free_slots;

Texture::Texture()
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

Texture::Texture(unsigned int size_x, unsigned int size_y):
	_size_x(size_x),
	_size_y(size_y)
{
	glGenTextures(1, &_gl_id);
	bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size_x, size_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::bind()
{
	glActiveTexture(GL_TEXTURE0 + _texture_slot);
	glBindTexture(GL_TEXTURE_2D, _gl_id);
}

void Texture::load_data()
{

}

}  // namespace bgfx