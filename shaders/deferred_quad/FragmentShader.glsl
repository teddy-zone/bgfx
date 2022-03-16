#version 430 core

layout(location = 0) out vec4 diffuseColor;

in vec2 uv;

uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform sampler2D color_tex;

void main()
{    
    vec3 light_loc = vec3(30.0,30.0,20.0);
    vec3 light_loc2 = vec3(50.0,50.0,25.0);

    vec3 norm = texture(normal_tex, uv).xyz;
    vec3 post = texture(position_tex, uv).xyz*100.0;
    vec3 r = light_loc - post;
    vec3 r2 = light_loc2 - post;

    float factor = 0;    
    //factor += 2000*clamp(dot(norm, normalize(r)), 0,1)/(length(r)*length(r));
    factor += 2000*clamp(dot(norm, normalize(r2)), 0,1)/(length(r2)*length(r2));
    diffuseColor = (factor)*vec4(1,1,1,1);
}  
