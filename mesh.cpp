#include "mesh.h"
#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "tiny_obj_loader.h"

#include <iostream>
#include <cassert>

namespace bgfx
{

Mesh::Mesh()
{
	_vao.add_buffer(dynamic_cast<BufferBase*>(&_vertices), "vpos", 3, bgfx::AttributeType::FLOAT);
	_vao.add_buffer(dynamic_cast<BufferBase*>(&_normals), "vnorm", 3, bgfx::AttributeType::FLOAT);
	_vao.add_buffer(dynamic_cast<BufferBase*>(&_uv), "vuv", 2, bgfx::AttributeType::FLOAT);
	_vao.add_buffer(dynamic_cast<BufferBase*>(&_vertex_color), "vcolor", 4, bgfx::AttributeType::FLOAT);
}

void Mesh::set_vertices(const std::vector<float>& in_vertices, bool do_calc_normals)
{
	if (do_calc_normals)
	{
		calc_normals(in_vertices);
	}
	_vertices.set_data(in_vertices);
}

void Mesh::set_uv_coords(const std::vector<float>& in_coords)
{
	_uv.set_data(in_coords);
}

void Mesh::set_vertex_colors(const std::vector<float>& in_colors)
{
	_vertex_color.set_data(in_colors);
}

void Mesh::calc_normals(const std::vector<float>& in_vertices)
{
	assert(in_vertices.size() % 3 == 0);
	std::vector<float> normals(in_vertices.size() / 3);
	for (size_t i = 0; i < in_vertices.size(); i += 3)
	{
		const glm::vec3* v1 = reinterpret_cast<const glm::vec3*>(&in_vertices[i]);
		const glm::vec3* v2 = reinterpret_cast<const glm::vec3*>(&in_vertices[i]) + 1;
		const glm::vec3* v3 = reinterpret_cast<const glm::vec3*>(&in_vertices[i]) + 2;
		glm::vec3* n = reinterpret_cast<glm::vec3*>(&normals[i / 3]);
		*n = glm::normalize(glm::cross(*v2 - *v1, *v3 - *v1));
	}
	_normals.set_data(normals);
}

int Mesh::triangle_count()
{
	return _vertices.get_size() / 9;
}

void Mesh::bind()
{
	_vao.bind();
}

void Mesh::load_obj(const std::string& in_file)
{
    std::string inputfile = in_file;
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "./"; // Path to material files

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(inputfile, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    std::vector<float> vertices;
    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                vertices.push_back(vx);
                vertices.push_back(vy);
                vertices.push_back(vz);
                // Check if `normal_index` is zero or positive. negative = no normal data
                if (idx.normal_index >= 0) {
                    tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
                    tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
                    tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
                }

                // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                if (idx.texcoord_index >= 0) {
                    tinyobj::real_t tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                    tinyobj::real_t ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                }

                // Optional: vertex colors
                // tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
                // tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
                // tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
            }
            index_offset += fv;

            // per-face material
            shapes[s].mesh.material_ids[f];
        }
    }
    set_vertices(vertices);
}

}  // namespace bgfx