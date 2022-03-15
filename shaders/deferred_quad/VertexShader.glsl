
#version 430 core
layout(location = 0) in vec3 vpos; // the position variable has attribute position 0
layout(location = 1) in vec3 vnorm; // the position variable has attribute position 0
layout(location = 2) in vec2 vuv; // the position variable has attribute position 0
layout(location = 3) in vec4 vcolor; // the position variable has attribute position 0

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform vec3 object_color;

out vec2 uv;

void main()
{
    gl_Position = vec4(vpos*2 - 1, 1);
    uv = vuv;
}
