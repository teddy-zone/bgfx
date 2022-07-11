#include "vertex_array.h"

#include <map>
#include <string>

#include "buffer.h"

#include "glad/gl.h"
#include <GLFW/glfw3.h>

namespace bgfx
{

VertexArray::VertexArray() :
	_buffer_indices(0)
{
#ifdef ENABLE_GRAPHICS
	glGenVertexArrays(1, &_gl_id);
#endif 
}

void VertexArray::bind()
{
#ifdef ENABLE_GRAPHICS
	glBindVertexArray(_gl_id);
#endif
}

void VertexArray::add_buffer(BufferBase* in_buf, std::string name, int size, AttributeType type, int stride, bool instanced)
{
	bind();
	in_buf->bind();
	GLuint buf_type;

	switch(type)
	{
	case AttributeType::INT:
		buf_type = GL_INT;
		break;
	case AttributeType::FLOAT:
		buf_type = GL_FLOAT;
		break;
	case AttributeType::DOUBLE:
		buf_type = GL_DOUBLE;
		break;
	case AttributeType::UNSIGNED_INT:
		buf_type = GL_UNSIGNED_INT;
		break;
	default:
		break;
	}

#ifdef ENABLE_GRAPHICS
	glVertexAttribPointer(_buffer_indices, size, buf_type, GL_FALSE, stride, nullptr);
	glEnableVertexAttribArray(_buffer_indices);
	if (instanced)
	{
		glVertexAttribDivisor(_buffer_indices, 1);
	}
#endif
	_buffers[name] = in_buf;
	_buffer_index[name] = _buffer_indices;
	_buffer_indices += 1;
	_instanced = true;
}

}  // namespace bgfx
