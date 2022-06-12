#type vertex
#version 410 core

layout (location=0) in vec2 aPos;
layout (location=1) in vec2 aUVS;

out vec2 fUVS;

uniform mat4 ortho;
uniform mat4 view;

void main(){
    fUVS = aUVS;
    gl_Position = ortho * view * vec4(floor(aPos.x), floor(aPos.y), 0.0, 1.0);
}

#type fragment
#version 410 core

uniform sampler2D texture_atlas;

in vec2 fUVS;
out vec4 fColor;

void main(){
    ivec2 texSize = textureSize(texture_atlas, 0);
    float s_offset = (1.0 / (float(texSize.x))) * 0.5;
    float t_offset = (1.0 / (float(texSize.y))) * 0.5;
    vec2 uv_final = vec2(fUVS.x + s_offset, fUVS.y + t_offset);
    fColor = texture(texture_atlas, uv_final);

       
}