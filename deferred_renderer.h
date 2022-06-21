#pragma once

#include <vector>
#include <unordered_map>

#include "point_light.h"
#include "decal.h"
#include "texture.h"
#include "framebuffer.h"
#include "material.h"
#include "renderable_mesh.h"

namespace bgfx
{

class DeferredRenderer
{
    int x_res, y_res;
    std::shared_ptr<bgfx::Framebuffer> g_buffer;
    std::shared_ptr<bgfx::Framebuffer> post_process_buffer;
    int post_process_offset;
    float res_factor = 2.5;

public:

    std::shared_ptr<bgfx::Texture> normal_pass_tex;
    std::shared_ptr<bgfx::Texture> color_pass_tex;
    std::shared_ptr<bgfx::Texture> position_pass_tex;
    std::shared_ptr<bgfx::Texture> object_id_pass_tex;
    std::shared_ptr<bgfx::Texture> depth_pass_tex;
    std::shared_ptr<bgfx::Texture> post_process_depth_pass_tex;

    std::vector<std::shared_ptr<bgfx::Texture>> post_process_textures;

    std::shared_ptr<bgfx::RenderableMesh> rmesh;
    std::shared_ptr<bgfx::Material> quad_mat;
    std::shared_ptr<bgfx::Material> gmat;
    std::shared_ptr<bgfx::Material> post_process_mat;

    bgfx::Buffer<bgfx::PointLight> point_light_buffer;
    bgfx::Buffer<bgfx::Decal> decal_buffer;

    std::vector<bgfx::PointLight> point_lights;
    std::unordered_map<int, bgfx::PointLight> point_light_map;
    std::vector<bgfx::Decal> decals;
    std::unordered_map<int, bgfx::Decal> decal_map;

    DeferredRenderer(int x_res, int y_res);

    virtual void draw();

    void bind_default();

    void bind_ssbo();

    void add_post_process_layer();

    void update_light_buffers();

    void update_decal_buffers();

};

}