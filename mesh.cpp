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
	_vao.add_buffer(dynamic_cast<BufferBase*>(&_vertex_indices), "vvind", 1, bgfx::AttributeType::UNSIGNED_INT);
}

void Mesh::set_vertices(const std::vector<float>& in_vertices, bool do_calc_normals)
{
	if (do_calc_normals)
	{
		calc_normals(in_vertices);
	}
	_vertices.set_data(in_vertices);
}

void Mesh::set_vertex_indices(const std::vector<unsigned int>& in_data)
{
    _vao.bind();
    _vertex_indices.set_data(in_data, BindPoint::ELEMENT_ARRAY_BUFFER);
}

void Mesh::set_normals(const std::vector<float>& in_normals)
{
    _normals.set_data(in_normals);
}

void Mesh::set_normal_indices(const std::vector<unsigned int>& in_data)
{
    _vao.bind();
    _normal_indices.set_data(in_data, BindPoint::ELEMENT_ARRAY_BUFFER);
}

void Mesh::set_uv_coords(const std::vector<float>& in_coords)
{
	_uv.set_data(in_coords);
}

void Mesh::set_vertex_colors(const std::vector<float>& in_colors)
{
	_vertex_color.set_data(in_colors);
}

void Mesh::set_solid_color(const glm::vec3& in_color)
{
    std::vector<float> color_vector;
    for (int i = 0; i < _saved_vertices.size()/3; ++i)
    {
        color_vector.push_back(in_color.x); 
        color_vector.push_back(in_color.y); 
        color_vector.push_back(in_color.z); 
        color_vector.push_back(1.0); 
    }
    set_vertex_colors(color_vector);
}

void Mesh::calc_normals(const std::vector<float>& in_vertices)
{
	assert(in_vertices.size() % 3 == 0);
	std::vector<float> normals(in_vertices.size());
	for (size_t i = 0; i < in_vertices.size(); i += 3)
	{
		const glm::vec3* v1 = reinterpret_cast<const glm::vec3*>(&in_vertices[i]);
		const glm::vec3* v2 = reinterpret_cast<const glm::vec3*>(&in_vertices[i]) + 1;
		const glm::vec3* v3 = reinterpret_cast<const glm::vec3*>(&in_vertices[i]) + 2;
		glm::vec3* n = reinterpret_cast<glm::vec3*>(&normals[i / 3]);
		*n = glm::normalize(glm::cross(*v2 - *v1, *v3 - *v1));
        *(n + 1) = glm::normalize(glm::cross(*v2 - *v1, *v3 - *v1));
        *(n+2) = glm::normalize(glm::cross(*v2 - *v1, *v3 - *v1));
	}
	_normals.set_data(normals);
}

int Mesh::triangle_count()
{
	return _vertices.get_size() / 9;
}

int Mesh::vertex_count()
{
    return _vertices.get_size() / 3;
}

void Mesh::bind()
{
	_vao.bind();
}

void Mesh::load_obj(const std::string& in_file, bool indexed)
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

    float min_x = 100000;
    float min_y = 100000;
    float min_z = 100000;
    float max_x = -100000;
    float max_y = -100000;
    float max_z = -100000;

    std::vector<float> vertices;
    auto attrib_vertices = attrib.vertices;
    std::vector<float> normals;
    std::vector<unsigned int> vertex_indices;
    std::vector<unsigned int> normal_indices;
    std::map<unsigned int, std::vector<unsigned int>> vertex_to_normal_index;
    std::map<unsigned int, unsigned int> vertex_to_count;

    if (indexed)
    { 
        for (size_t s = 0; s < shapes.size(); s++) {
            int max_index = 0;
            for (auto& index : shapes[s].mesh.indices)
            {
                if (index.vertex_index > max_index)
                {
                    max_index = index.vertex_index;
                }
                if (0 && vertex_to_normal_index.find(index.vertex_index) != vertex_to_normal_index.end())
                {
                    attrib_vertices.push_back(attrib.vertices[3*index.vertex_index + 0]);
                    attrib_vertices.push_back(attrib.vertices[3*index.vertex_index + 1]);
                    attrib_vertices.push_back(attrib.vertices[3*index.vertex_index + 2]);
                    vertex_indices.push_back(int(attrib_vertices.size()/3 - 1));
                }
                else
                {
                    vertex_indices.push_back(int(index.vertex_index));
                }
                auto vx = attrib.vertices[3*index.vertex_index + 0];
                auto vy = attrib.vertices[3*index.vertex_index + 1];
                auto vz = attrib.vertices[3*index.vertex_index + 2];
                if (vx < min_x) { min_x = vx; }
                if (vy < min_y) { min_y = vy; }
                if (vz < min_z) { min_z = vz; }
                if (vx > max_x) { max_x = vx; }
                if (vy > max_y) { max_y = vy; }
                if (vz > max_z) { max_z = vz; }
                _saved_vertices.push_back(vx);
                _saved_vertices.push_back(vy);
                _saved_vertices.push_back(vz);
                vertex_to_normal_index[index.vertex_index].push_back(index.normal_index);
                if (vertex_to_count.find(vertex_indices.back()) == vertex_to_count.end())
                {
                    vertex_to_count[vertex_indices.back()] = 1;
                }
                else
                {
                    vertex_to_count[vertex_indices.back()] += 1;
                }
            }
        }
        _indexed = true;
        normals.resize(attrib_vertices.size(), 0);
        for (int i = 0; i < attrib_vertices.size()/3; ++i)
        {
            auto normal_indices = vertex_to_normal_index[i];
            for (auto& normal_index : normal_indices )
            {
                auto nx = (attrib.normals[3*normal_index + 0]);
                auto ny = (attrib.normals[3*normal_index + 1]);
                auto nz = (attrib.normals[3*normal_index + 2]);
                normals[i*3 + 0] += nx;
                normals[i*3 + 1] += ny;
                normals[i*3 + 2] += nz;
            }
        }
        for (int i = 0; i < attrib_vertices.size()/3; ++i)
        {
            auto nx = normals[i*3 + 0];
            auto ny = normals[i*3 + 1];
            auto nz = normals[i*3 + 2];
            auto n = glm::normalize(glm::vec3(nx, ny, nz));
            normals[i*3 + 0] = n.x; 
            normals[i*3 + 1] = n.y; 
            normals[i*3 + 2] = n.z; 
        }
        _bmin = glm::vec3(min_x, min_y, min_z);
        _bmax = glm::vec3(max_x, max_y, max_z);
        set_vertices(attrib_vertices);
        set_normals(normals);
        set_vertex_indices(vertex_indices);
    }
    else
    {
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

                    if (vx < min_x) { min_x = vx; }
                    if (vy < min_y) { min_y = vy; }
                    if (vz < min_z) { min_z = vz; }
                    if (vx > max_x) { max_x = vx; }
                    if (vy > max_y) { max_y = vy; }
                    if (vz > max_z) { max_z = vz; }

                    vertices.push_back(vx);
                    vertices.push_back(vy);
                    vertices.push_back(vz);
                    _saved_vertices.push_back(vx);
                    _saved_vertices.push_back(vy);
                    _saved_vertices.push_back(vz);
                    // Check if `normal_index` is zero or positive. negative = no normal data
                    if (idx.normal_index >= 0) {
                        tinyobj::real_t nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
                        tinyobj::real_t ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
                        tinyobj::real_t nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
                        normals.push_back(nx);
                        normals.push_back(ny);
                        normals.push_back(nz);
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
        _bmin = glm::vec3(min_x, min_y, min_z);
        _bmax = glm::vec3(max_x, max_y, max_z);
        _indexed = false;
        set_vertices(vertices);
        set_normals(normals);

    }
    set_solid_color(glm::vec3(1,1,1));
    //set_normal_indices(normal_indices);
}

}  // namespace bgfx
