#pragma once

#include <map>
#include <string>

#include "buffer.h"

namespace bgfx
{

enum class AttributeType
{
	FLOAT,
	INT,
	UNSIGNED_INT,
	DOUBLE,
};

class VertexArray
{
protected:

	unsigned int _gl_id;
	std::map<std::string, BufferBase*> _buffers;
	std::map<std::string, int> _buffer_index;
	int _buffer_indices;

public:
	VertexArray();

	virtual void bind();
	void add_buffer(BufferBase* in_buf, std::string name, int size, AttributeType type, int stride = 0);

};

}  // namespace bgfx

