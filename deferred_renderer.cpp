#include <filesystem>

#include "glad/gl.h"

#include "deferred_renderer.h"
#include "shaders/main_deferred_fragment_shader.glsl.h"
#include "shaders/main_deferred_vertex_shader.glsl.h"
#include "shaders/deferred_quad_vertex_shader.glsl.h"
#include "shaders/deferred_quad_fragment_shader.glsl.h"

namespace bgfx
{

DeferredRenderer::DeferredRenderer(int in_x_res, int in_y_res):
    x_res(in_x_res),
    y_res(in_y_res)
{
#ifdef ENABLE_GRAPHICS
    post_process_offset = 3; // Number of non-post process textures
    normal_pass_tex = std::make_shared<bgfx::Texture>("normal", x_res/res_factor, y_res/res_factor);
    color_pass_tex = std::make_shared<bgfx::Texture>("color", x_res/res_factor, y_res/res_factor);
    position_pass_tex = std::make_shared<bgfx::Texture>("position", x_res/res_factor, y_res/res_factor);
    object_id_pass_tex = std::make_shared<bgfx::Texture>("object_id", x_res / res_factor, y_res / res_factor);
    depth_pass_tex = std::make_shared<bgfx::Texture>("depth", x_res/res_factor, y_res/res_factor);
    normal_pass_tex->to_render(x_res/res_factor, y_res/res_factor);
    color_pass_tex->to_render(x_res/res_factor, y_res/res_factor);
    position_pass_tex->to_render(x_res/res_factor, y_res/res_factor);
    object_id_pass_tex->to_render(x_res/res_factor, y_res/res_factor);
    depth_pass_tex->to_depth(x_res/res_factor, y_res/res_factor);
    g_buffer = std::make_shared<bgfx::Framebuffer>("g_buffer", x_res, y_res);
    g_buffer->bind();
    
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        printf("GL ERROR!: %d", err);
		throw "opengl error";
    }
    std::vector<unsigned int> attachments = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4, attachments.data());
    g_buffer->attach_texture(position_pass_tex, 0);
    g_buffer->attach_texture(normal_pass_tex, 1);
    g_buffer->attach_texture(color_pass_tex, 2);
    g_buffer->attach_texture(object_id_pass_tex, 3);
    g_buffer->attach_depth_texture(depth_pass_tex);


    while ((err = glGetError()) != GL_NO_ERROR)
    {
        printf("GL ERROR!: %d", err);
		throw "opengl error";
    }
        
#endif

    gmat = std::make_shared<bgfx::Material>(deferred_quad_vertex_shader,
        deferred_quad_fragment_shader, true);
    instanced_gmat = std::make_shared<bgfx::Material>(deferred_quad_vertex_shader_instanced,
        deferred_quad_fragment_shader, true);
    quad_mat = std::make_shared<bgfx::Material>(main_deferred_vertex_shader, main_deferred_fragment_shader, true);

    rmesh = std::make_shared<bgfx::RenderableMesh>();
    auto mesh = std::make_shared<bgfx::Mesh>();
    rmesh->set_mesh(mesh);
    rmesh->get_mesh()->set_vertices({ {1,0,0, 0,1,0, 0,0,0, 0,1,0, 1,1,0, 1,0,0} });
    rmesh->get_mesh()->set_uv_coords({ {1,0, 0,1, 0,0, 0,1, 1,1, 1,0} });
    rmesh->get_mesh()->set_vertex_indices({ {0,1,2,3,4,5} });
    rmesh->set_material(quad_mat);
    point_lights.push_back({ {10,10,50,1}, {1,0.9,0.8,1}, 0.1, 1, {0,0}});
    point_lights.push_back({ {100,50,50,1}, {0.9,1,1,1}, 0.2, 1, {0,0} });
    Decal test_decal;
    test_decal.color = glm::vec4(0,0,1,1);
    test_decal.location = glm::vec4(50,50,50,1);
    test_decal.angle = 2*3.1415926;
    test_decal.radius = 10.0;
    test_decal.type = 1;
    test_decal.t = 0.0;
    decals.push_back(test_decal);
    bind_default();
    quad_mat->use();
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, point_light_buffer.get_id());
    point_light_buffer.set_data(point_lights, BindPoint::SHADER_STORAGE_BUFFER);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, decal_buffer.get_id());
    decal_buffer.set_data(decals, BindPoint::SHADER_STORAGE_BUFFER);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, fow_buffer.get_id());
    fow_buffer.set_data(fows, BindPoint::SHADER_STORAGE_BUFFER);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, spotlight_buffer.get_id());
    spotlight_buffer.set_data(spotlights, BindPoint::SHADER_STORAGE_BUFFER);

    quad_mat->set_uniform_i1("point_light_count", point_lights.size());
    quad_mat->set_uniform_i1("decal_count", decals.size());
    quad_mat->set_uniform_i1("selected_object", 1);
    quad_mat->set_uniform_i1("spotlight_buffer", spotlights.size());
    
    //glShaderStorageBlockBinding(quad_mat->get_program_id(), block_index, 2);

    
}

void DeferredRenderer::draw(bool clear)
{
    glViewport(0,0,x_res/res_factor, y_res/res_factor);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    g_buffer->bind();
    if (clear)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    //std::vector<unsigned int> attachments = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    //glDrawBuffers(3, attachments.data());
    gmat->use();

}

void DeferredRenderer::draw_instanced(bool clear)
{
    glViewport(0, 0, x_res / res_factor, y_res / res_factor);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    g_buffer->bind();
    if (clear)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }   
    //std::vector<unsigned int> attachments = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    //glDrawBuffers(3, attachments.data());
    instanced_gmat->use();

}


void DeferredRenderer::bind_default()
{
	glViewport(0,0,x_res, y_res);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRenderer::bind_ssbo() 
{ 
    //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, point_light_buffer.get_id()); 
    //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, decal_buffer.get_id()); 
}

void DeferredRenderer::add_post_process_layer()
{ 
    post_process_textures.push_back(std::make_shared<bgfx::Texture>("pptex" + std::to_string(post_process_textures.size()), x_res, y_res));
    post_process_textures.back()->to_render(x_res, y_res);
    int num_attachments = post_process_offset + post_process_textures.size();
    g_buffer->attach_texture(post_process_textures.back(), num_attachments);
    std::vector<unsigned int> attachments;// = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    for (int i = 0; i < num_attachments; ++i)
    {
        attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
    }
    glDrawBuffers(num_attachments, attachments.data());
}

void DeferredRenderer::update_light_buffers()
{
    point_lights.clear();
    point_lights.reserve(point_light_map.size());
    for (auto& [light_name, light] : point_light_map)
    {
        point_lights.push_back(light);
    }

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, point_light_buffer.get_id());
    point_light_buffer.set_data(point_lights, BindPoint::SHADER_STORAGE_BUFFER);
    quad_mat->use();
    quad_mat->set_uniform_i1("point_light_count", point_lights.size());

    spotlights.clear();
    spotlights.reserve(spotlight_map.size());
    for (auto& [light_name, light] : spotlight_map)
    {
        spotlights.push_back(light);
    }

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, spotlight_buffer.get_id());
    spotlight_buffer.set_data(spotlights, BindPoint::SHADER_STORAGE_BUFFER);
    quad_mat->use();
    quad_mat->set_uniform_i1("spotlight_count", spotlights.size());
}

void DeferredRenderer::update_decal_buffers()
{
    decals.clear();
    decals.reserve(decal_map.size());
    for (auto& [decal_name, decal] : decal_map)
    {
        decals.push_back(decal);
    }
    GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("GL ERROR!: %d", err);
		throw "opengl error";
	}

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, decal_buffer.get_id());
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		printf("GL ERROR!: %d", err);
		throw "opengl error";
	}
    decal_buffer.set_data(decals, BindPoint::SHADER_STORAGE_BUFFER);

    quad_mat->use();
    quad_mat->set_uniform_i1("decal_count", decals.size());
}

void DeferredRenderer::update_fow_buffers()
{
    fows.clear();
    fows.reserve(fow_map.size());
    for (auto& [fow_name, fow] : fow_map)
    {
        fows.push_back(fow);
    }
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        printf("GL ERROR!: %d", err);
        throw "opengl error";
    }

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, fow_buffer.get_id());
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        printf("GL ERROR!: %d", err);
        throw "opengl error";
    }
    fow_buffer.set_data(fows, BindPoint::SHADER_STORAGE_BUFFER);
    
    quad_mat->use();
    quad_mat->set_uniform_i1("fow_count", fows.size());
    
}

}
