#pragma once

#include <cstddef>

namespace bgfx
{

enum class BindPoint
{
	PIXEL_READ_BUFFER,
	PIXEL_WRITE_BUFFER,
	ARRAY_BUFFER,
	UNIFORM_BUFFER,
	TEXTURE_BUFFER,
	SHADER_STORAGE_BUFFER,
	ELEMENT_ARRAY_BUFFER,
};

class RawBuffer
{
	int _size;

public:
	unsigned int _gl_id;
	explicit RawBuffer();
	~RawBuffer();
	void bind(BindPoint bind_point=BindPoint::ARRAY_BUFFER);
	void buffer_data(const void* data, size_t element_count, size_t element_size, BindPoint bind_point=BindPoint::ARRAY_BUFFER);
	void buffer_storage(const void* data, size_t element_count, size_t element_size);
	void buffer_sub_data(const void* data, size_t data_start, size_t element_count, size_t element_size);
	void* map_data();
	int get_size();
	void get_data(void* out_data, size_t element_count = 0, size_t element_size = 1, size_t data_start = 0);

};

}  // namespace bgfx
