#pragma once
#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <vector>

#include "glad/gl.h"
#include <GLFW/glfw3.h>

#include "gl_object.h"

//class VertexArray;

enum class BufferType
{
	SSBO,
	VBO,
	TBO,
	UBO
};

class BufferGeneric : public GLObject
{
protected:

	friend class VertexArray;
public:
	virtual void bind(GLuint bindpoint = -1) override
	{
		if (bindpoint == -1)
		{
			bindpoint = get_bindpoint();
		}
		glBindBuffer(bindpoint, get_gl_id());
	}
	BufferGeneric(BufferType btype=BufferType::VBO)
	{
		glGenBuffers(1, &get_gl_id());
		switch (btype)
		{
		case BufferType::VBO:
			this->set_bindpoint(GL_ARRAY_BUFFER);
			break;
		case BufferType::SSBO:
			this->set_bindpoint(GL_SHADER_STORAGE_BUFFER);
			break;
		case BufferType::TBO:
			this->set_bindpoint(GL_TEXTURE_BUFFER);
			break;
		case BufferType::UBO:
			this->set_bindpoint(GL_UNIFORM_BUFFER);
			break;
		default:
			throw "Unkown buffer type!";
		}
	}

	//virtual void clear() = 0;
};

template <class DataType>
class BufferBase : public BufferGeneric
{
public:
	BufferBase(BufferType in_type):
		BufferGeneric(in_type)
	{}
	//virtual BufferBase copy();
	//virtual void copy(BufferBase& dest_buffer);
};

template <class DataType>
class BufferImmutable : public BufferBase<DataType>
{
private:
	const GLbitfield _flags;
	
public:
	BufferImmutable(const std::vector<DataType>& in_data, BufferType btype=BufferType::VBO, GLbitfield flags=0):
		_flags(flags),
		BufferGeneric(btype)
	{
		this->bind();
		glBufferStorage(this->get_bindpoint(),
			in_data.size()*sizeof(DataType),
			reinterpret_cast<void*>(in_data.data()),
			flags);
	}

};

template <class DataType>
class Buffer : public BufferBase<DataType>
{
private:
	GLuint _usage;
public:
	Buffer(const std::vector<DataType>& in_data, BufferType btype=BufferType::VBO, GLuint usage=GL_DYNAMIC_DRAW):
		_usage(usage),
		BufferBase<DataType>(btype)
	{
		this->bind();
		glBufferData(this->get_bindpoint(),
			in_data.size() * sizeof(DataType),
			reinterpret_cast<const void*>(const_cast<const DataType*>(in_data.data())),
			usage);
	}

	void set_data(const std::vector<DataType>& in_data, BufferType btype = BufferType::VBO, GLuint usage = GL_DYNAMIC_DRAW)
	{
		this->bind();
		glBufferData(this->get_bindpoint(),
			in_data.size() * sizeof(DataType),
			reinterpret_cast<const void*>(const_cast<const DataType*>(in_data.data())),
			usage);
	}
};

#endif  // _BUFFER_H_