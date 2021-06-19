#include "material.h"

namespace bgfx
{

std::shared_ptr<Material::NodeOutput> Material::_uv_node_out = std::make_shared<NodeOutput>("uv");
std::shared_ptr<Material::NodeOutput> Material::_pos_node_out = std::make_shared<NodeOutput>("pos");
std::shared_ptr<Material::NodeOutput> Material::_normal_node_out = std::make_shared<NodeOutput>("norm");

std::string default_mat_vertex_shader = R"V0G0N(

#version 430 core
layout (location = 0) in vec3 vpos; // the position variable has attribute position 0
layout (location = 1) in vec3 vnorm; // the position variable has attribute position 0
layout (location = 2) in vec2 vuv; // the position variable has attribute position 0
 
uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

out vec4 vertex_color; // specify a color output to the fragment shader
out vec2 uv;

void main()
{
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vpos, 1.0);
    //gl_Position = vec4(vpos, 1.0);
    vertex_color = vec4(0.5, 0.0, 0.0, 1.0); // set the output variable to a dark-red color
    uv = vuv;
}

)V0G0N";

std::string default_mat_fragment_shader = R"V0G0N(

#version 430 core

in vec4 vertex_color; // specify a color output to the fragment shader
in vec3 pos; // specify a color output to the fragment shader
in vec3 norm; // specify a color output to the fragment shader
in vec2 uv;

//_TEX_SAMPLERS_

void main()
{
    //_TEX_EXECUTION_
    gl_FragColor = vertex_color;
}

)V0G0N";

}  // namespace bgfx