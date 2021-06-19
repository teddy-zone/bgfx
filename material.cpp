#include "material.h"

namespace bgfx
{

std::string default_mat_vertex_shader = R"V0G0N(

#version 430 core
layout (location = 0) in vec3 vpos; // the position variable has attribute position 0
layout (location = 1) in vec3 vnorm; // the position variable has attribute position 0
 
uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

out vec4 vertex_color; // specify a color output to the fragment shader

void main()
{
    //gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vpos, 1.0);
    gl_Position = vec4(vpos, 1.0);
    vertex_color = vec4(0.5, 0.0, 0.0, 1.0); // set the output variable to a dark-red color
}

)V0G0N";

std::string default_mat_fragment_shader = R"V0G0N(

#version 430 core

in vec4 vertex_color; // specify a color output to the fragment shader

void main()
{
    gl_FragColor = vertex_color;
}

)V0G0N";

}  // namespace bgfx