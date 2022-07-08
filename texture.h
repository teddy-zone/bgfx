#pragma once

#include <deque>
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace bgfx
{

class Texture
{


	unsigned int _gl_id;
	unsigned int _size_x;
	unsigned int _size_y;
	int _texture_slot;
	static int _current_texture_slot;
	static std::deque<int> _free_slots;
	std::string _name;
	std::vector<float> _data;

public:
	enum class InterpMode
	{
		LINEAR,
		NEAREST
	};

	Texture(const std::string& name);
	~Texture();
	Texture(const std::string& name, unsigned int size_x, unsigned int size_y);

	void bind();
    void load_data(unsigned char* data, int in_size_x, int in_size_y);
	void to_render(int in_size_x, int in_size_y);
	void to_depth(int in_size_x, int in_size_y);
	const std::string& name();
	unsigned int slot() const { return _texture_slot; }
	void set_slot(unsigned int in_slot) { _texture_slot = in_slot; }
	unsigned int gl_id() { return _gl_id; }
	void set_interpolation_mode(InterpMode interp_mode);
	void write_to_region(int top_left_x, int top_left_y, int width, int height, const std::vector<char>& data);
	void read_data_to_memory();
	glm::vec3 get_pixel(int x, int y);

};

}  // namespace bgfx
