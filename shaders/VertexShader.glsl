
#version 430 core
layout(location = 0) in vec3 vpos; // the position variable has attribute position 0
layout(location = 1) in vec3 vnorm; // the position variable has attribute position 0
layout(location = 2) in vec2 vuv; // the position variable has attribute position 0
layout(location = 3) in vec4 vcolor; // the position variable has attribute position 0

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;
uniform vec3 object_color;
uniform int object_id;

out vec4 color; // specify a color output to the fragment shader
out vec2 uv;
out vec3 norm;
out vec4 pos;
flat out int f_object_id;

void main()
{
    vec4 interpos = model_matrix * vec4(vpos, 1.0);

    gl_Position = projection_matrix * view_matrix * interpos;
    pos = interpos/100.0;

    norm = mat3(transpose(inverse(model_matrix)))*vnorm;
    color = vcolor;//vec4(object_color, 1); // set the output variable to a dark-red color
    f_object_id = object_id;
}
