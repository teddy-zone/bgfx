#include <string>

inline std::string deferred_quad_fragment_shader = R"SHAD(
#version 460 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec4 gObjectId;

in vec2 uv;
in vec4 pos;
in vec3 norm;
in vec4 color;
flat in int f_object_id;


void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = pos.xyz;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(norm);
    // and the diffuse per-fragment color
    gAlbedoSpec = color;
    if (color.a < 0.99 && color.a > 0.5)
    {
        const int dither_number = int(10*(1 - color.a));
        if (int(gl_FragCoord.x) % dither_number == 0)
        {
            discard;
        }
    }
    if (color.a < 0.5)
    {
        const int dither_number = int(10*(color.a));
        if (int(gl_FragCoord.x) % dither_number != 0)
        {
            discard;
        }
    }
    gObjectId = vec4(float(f_object_id), 0, 0,0);
    // store specular intensity in gAlbedoSpec's alpha component
}  
)SHAD";