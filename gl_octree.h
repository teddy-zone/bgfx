#pragma once
#ifndef _GL_OCTREE_H_
#define _GL_OCTREE_H_

#include "glm/glm.hpp"
#include "buffer.h"
#include "GL/gl.h"

namespace bgfx
{

struct GLOctreeNode
{
	GLuint data = 0;
	GLuint children[8];
	//GLuint pad1[7];
};

inline GLOctreeNode create_node(GLbyte in_data = 0)
{
	GLOctreeNode out_node;
	for (size_t i = 0; i < 8; ++i)
	{
		out_node.children[i] = 0;
	}
	out_node.data = in_data;
	return std::move(out_node);
}

inline unsigned int vec_to_child(glm::vec3 node_loc, float node_size, glm::vec3 loc)
{
	glm::vec3 node_center = node_loc + glm::vec3(node_size) / 2.0f;
	return int(bool(loc.x > node_center.x)) * 1 + int(bool(loc.y > node_center.y)) * 2 + int(bool(loc.z > node_center.z)) * 4;
}

inline glm::vec3 child_to_vec(unsigned int child)
{
	return (glm::vec3(int(bool(child & 1)), int(bool(child & 2)), int(bool(child & 4))));
}

inline unsigned int place_node(std::vector<GLOctreeNode>& nodes, unsigned int root_node, glm::vec3 root_loc, float root_size, glm::vec3 loc, float size, int data, bool place_inside = false)
{
	if (nodes[root_node].data > 0 && !place_inside)
	{
		return -1;
	}
	if (size - root_size > 0)
	{
		throw "Trying to place node in smaller node!";
	}
	if (std::abs(size - root_size) < root_size * 0.0001)
	{
		nodes[root_node].data = data;
		return root_node;
	}
	else
	{
		auto child_index = vec_to_child(root_loc, root_size, loc);
		auto child_node = nodes[root_node].children[child_index];
		if (child_node <= 0)
		{
			nodes.push_back(create_node());
			nodes[root_node].children[child_index] = nodes.size() - 1;
			child_node = nodes.size() - 1;
		}
		auto child_loc = root_loc + child_to_vec(child_index) * root_size / 2.0f;
		return place_node(nodes, child_node, child_loc, root_size / 2.0, loc, size, data);
	}
}

//inline void place_node()

class GLOctree
{


public:
	GLfloat root_size;
	std::vector<GLOctreeNode> nodes;
	Buffer<GLOctreeNode>* node_buffer;
	GLOctree(GLfloat in_size) :
		root_size(in_size)
	{
		root_size = in_size;
		nodes.push_back(create_node());

		node_buffer = new Buffer<GLOctreeNode>(nodes);
	}

	void refresh_data()
	{
		node_buffer->set_data(nodes);
	}
};

}  // namespace bgfx

#endif  // _GL_OCTREE_H_
