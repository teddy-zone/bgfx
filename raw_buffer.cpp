#include "raw_buffer.h"

#include <stdio.h>

#include "glad/gl.h"
#include <GLFW/glfw3.h>



namespace bgfx
{

GLenum bind_point_to_enum(BindPoint bind_point)
{
	GLenum gl_bind_point;
	switch (bind_point)
	{
	case BindPoint::ARRAY_BUFFER:
		gl_bind_point = GL_ARRAY_BUFFER;
		break;
	case BindPoint::PIXEL_READ_BUFFER:
		gl_bind_point = GL_PIXEL_PACK_BUFFER;
		break;
	case BindPoint::PIXEL_WRITE_BUFFER:
		gl_bind_point = GL_PIXEL_UNPACK_BUFFER;
		break;
	case BindPoint::SHADER_STORAGE_BUFFER:
		gl_bind_point = GL_SHADER_STORAGE_BUFFER;
		break;
	case BindPoint::TEXTURE_BUFFER:
		gl_bind_point = GL_TEXTURE_BUFFER;
		break;
	case BindPoint::UNIFORM_BUFFER:
		gl_bind_point = GL_UNIFORM_BUFFER;
		break;
	default:
		break;
	}
	return gl_bind_point;
}

RawBuffer::RawBuffer():
	_size(0)
{
#ifdef ENABLE_GRAPHICS
	glGenBuffers(1, &_gl_id);
#endif 
}

RawBuffer::~RawBuffer()
{
#ifdef ENABLE_GRAPHICS
	glDeleteBuffers(1, &_gl_id);
#endif
}

void RawBuffer::bind(BindPoint bind_point)
{
	GLuint gl_bind_point = bind_point_to_enum(bind_point);
#ifdef ENABLE_GRAPHICS
	glBindBuffer(gl_bind_point, _gl_id);
#endif 
}

void RawBuffer::buffer_data(const void* data, size_t element_count, size_t element_size, BindPoint bind_point)
{
	GLuint gl_bind_point = bind_point_to_enum(bind_point);
	bind(bind_point);
#ifdef ENABLE_GRAPHICS
	glBufferData(gl_bind_point, element_count * element_size, data, GL_DYNAMIC_DRAW);
#endif
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("GL ERROR!: %d", err);
	}
	_size = element_count * element_size;
}

void RawBuffer::buffer_storage(const void* data, size_t element_count, size_t element_size)
{
	bind(BindPoint::ARRAY_BUFFER);
#ifdef ENABLE_GRAPHICS
	glBufferStorage(GL_ARRAY_BUFFER, element_count * element_size, data, GL_DYNAMIC_DRAW);
#endif
	_size = element_count * element_size;
}

void RawBuffer::buffer_sub_data(const void* data, size_t data_start, size_t element_count, size_t element_size)
{
	bind(BindPoint::ARRAY_BUFFER);
#ifdef ENABLE_GRAPHICS
	glBufferSubData(GL_ARRAY_BUFFER, data_start * element_size, element_count * element_size, data);
#endif
}

void* RawBuffer::map_data()
{
	bind(BindPoint::ARRAY_BUFFER);
#ifdef ENABLE_GRAPHICS
	return glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
#endif 
    return nullptr;
}

int RawBuffer::get_size()
{
	return _size;
}

void RawBuffer::get_data(void* out_data, size_t element_count, size_t element_size, size_t data_start)
{
	if (element_count == -1) { element_count = _size; }
	bind(BindPoint::ARRAY_BUFFER);
#ifdef ENABLE_GRAPHICS
	glGetBufferSubData(GL_ARRAY_BUFFER, data_start, element_size * element_count, out_data);
#endif 
}

}  // namespace bgfx
