#pragma once
#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <vector>

#include "raw_buffer.h"

namespace bgfx
{

// Base class for all templated Buffers to inherit from same class
class BufferBase
{
protected:
	RawBuffer _raw_buffer;
public:
	void bind()
	{
		_raw_buffer.bind();
	}
	unsigned int get_id()
	{
		return _raw_buffer._gl_id;
	}
};

// Generic data type type-safe buffer class
template <class DataType>
class Buffer : public BufferBase
{
	DataType* _mapped_data = nullptr;
	std::vector<DataType> _local_copy;
	std::vector<DataType> _local_range_copy;

public:
	explicit Buffer()
	{
	}

	explicit Buffer(const std::vector<DataType>& in_data)
	{
		set_data(in_data);
	}

	void set_data(const std::vector<DataType>& in_data, BindPoint bind_point=BindPoint::ARRAY_BUFFER)
	{
		_raw_buffer.buffer_data(in_data.data(), in_data.size(), sizeof(DataType));
	}

	void set_data_range(const std::vector<DataType>& in_data, size_t start_element)
	{
		_raw_buffer.buffer_sub_data(in_data.data(), start_element, in_data.size(), sizeof(DataType));
	}

	std::vector<DataType>& get_data() const
	{
		_raw_buffer.get_data(_local_copy.data());
	}

	std::vector<DataType>& get_data_range(size_t start_element, size_t element_count) const
	{
		_raw_buffer.get_data(_local_copy.data(), element_count, sizeof(DataType), start_element);
	}

	void map_data()
	{
		_mapped_data = reinterpret_cast<DataType*>(_raw_buffer.map_data());
	}

	DataType operator[](size_t index)
	{
		if (!_mapped_data)
		{
			map_data();
		}
		assert(_raw_buffer.get_size() / sizeof(DataType) > index);
		return _mapped_data[index];
	}

	int get_size()
	{
		auto el_size = _raw_buffer.get_size()/sizeof(DataType);
		return el_size;
	}
};

}


#endif  // _BUFFER_H_
