#include "framebuffer.h"

#include <iostream>

#include "glad/gl.h"
#include <GLFW/glfw3.h>

namespace bgfx
{

	Framebuffer::Framebuffer(const std::string& name) :
		_name(name)
	{
		glGenFramebuffers(1, &_gl_id);
	}

	Framebuffer::~Framebuffer()
	{
		glDeleteFramebuffers(1, &_gl_id);
	}

	Framebuffer::Framebuffer(const std::string& name, unsigned int size_x, unsigned int size_y) :
		_size_x(size_x),
		_size_y(size_y),
		_name(name)
	{
		glGenFramebuffers(1, &_gl_id);
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			printf("GL ERROR!: %d", err);
		}
	}

	void Framebuffer::bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, _gl_id);
	}

	const std::string& Framebuffer::name()
	{
		return _name;
	}

	void Framebuffer::attach_texture(std::shared_ptr<Texture>& in_tex)
	{
		bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, in_tex->gl_id(), 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

}  // namespace bgfx
