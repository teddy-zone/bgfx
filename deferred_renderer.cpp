#include "glad/gl.h"

#include "deferred_renderer.h"

namespace bgfx
{

DeferredRenderer::DeferredRenderer(int in_x_res, int in_y_res):
    x_res(in_x_res),
    y_res(in_y_res)
{
#ifdef ENABLE_GRAPHICS
    post_process_offset = 3; // Number of non-post process textures
    
    normal_pass_tex = std::make_shared<bgfx::Texture>("normal", x_res, y_res);
    color_pass_tex = std::make_shared<bgfx::Texture>("color", x_res, y_res);
    position_pass_tex = std::make_shared<bgfx::Texture>("position", x_res, y_res);
    object_id_pass_tex = std::make_shared<bgfx::Texture>("object_id", x_res, y_res);
    depth_pass_tex = std::make_shared<bgfx::Texture>("depth", x_res, y_res);
    normal_pass_tex->to_render(x_res, y_res);
    color_pass_tex->to_render(x_res, y_res);
    position_pass_tex->to_render(x_res, y_res);
    object_id_pass_tex->to_render(x_res, y_res);
    depth_pass_tex->to_depth(x_res, y_res);
    g_buffer = std::make_shared<bgfx::Framebuffer>("g_buffer", x_res, y_res);
    g_buffer->bind();
    
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        printf("GL ERROR!: %d", err);
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
    }
        
#endif
    quad_mat = std::make_shared<bgfx::Material>("C:\\Users\\tjwal\\projects\\game_stuff\\bare_gfx\\shaders\\deferred_quad\\VertexShader.glsl", 
        "C:\\Users\\tjwal\\projects\\game_stuff\\bare_gfx\\shaders\\deferred_quad\\FragmentShader.glsl");

    rmesh = std::make_shared<bgfx::RenderableMesh>();
    auto mesh = std::make_shared<bgfx::Mesh>();
    rmesh->set_mesh(mesh);
    rmesh->get_mesh()->set_vertices({ {1,0,0, 0,1,0, 0,0,0, 0,1,0, 1,1,0, 1,0,0} });
    rmesh->get_mesh()->set_uv_coords({ {1,0, 0,1, 0,0, 0,1, 1,1, 1,0} });
    rmesh->get_mesh()->set_vertex_indices({ {0,1,2,3,4,5} });
    rmesh->set_material(quad_mat);
    point_lights.push_back({ {10,10,50,1}, {1,0.9,0.8,1}, 0.1, {0,0,0} });
    point_lights.push_back({ {100,50,50,1}, {0.9,1,1,1}, 0.2, {0,0,0} });
    gmat = std::make_shared<bgfx::Material>("C:\\Users\\tjwal\\projects\\game_stuff\\bare_gfx\\shaders\\VertexShader.glsl", "C:\\Users\\tjwal\\projects\\game_stuff\\bare_gfx\\shaders\\deferred_shader.glsl");
    post_process_mat = std::make_shared<bgfx::Material>("C:\\Users\\tjwal\\projects\\game_stuff\\bare_gfx\\shaders\\VertexShader.glsl", "C:\\Users\\tjwal\\projects\\game_stuff\\bare_gfx\\shaders\\deferred_shader.glsl");
    bind_default();
    quad_mat->use();
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, point_light_buffer.get_id());
    point_light_buffer.set_data(point_lights, BindPoint::SHADER_STORAGE_BUFFER);
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    //auto blok_index = glGetProgramResourceIndex(quad_mat->get_program_id(), GL_SHADER_STORAGE_BLOCK, "point_lights_uni");
    point_light_buffer.bind();
    GLint data;
    glGetBufferParameteriv(GL_SHADER_STORAGE_BUFFER, GL_BUFFER_USAGE, &data);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    quad_mat->set_uniform_i1("point_light_count", point_lights.size());
    quad_mat->set_uniform_i1("selected_object", 1);
    
    //glShaderStorageBlockBinding(quad_mat->get_program_id(), block_index, 2);

    
}

void DeferredRenderer::draw()
{
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    g_buffer->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //std::vector<unsigned int> attachments = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    //glDrawBuffers(3, attachments.data());
    gmat->use();
    
}


void DeferredRenderer::bind_default()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRenderer::bind_ssbo() { glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, point_light_buffer.get_id()); }

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

}