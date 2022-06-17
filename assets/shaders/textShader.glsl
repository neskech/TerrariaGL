#type vertex
#version 410 core

layout (location=0) in vec2 aPos;
layout (location=1) in vec2 aUVs;
layout (location=2) in vec3 aColor;

out vec2 fUVs;
out vec3 fColor;

uniform mat4 ortho;
uniform mat4 view;

void main(){
    fColor = aColor;
    fUVs = aUVs;
    gl_Position = ortho * view * vec4(aPos.x, aPos.y, 9.0, 1.0);
}

#type fragment
#version 410 core

uniform sampler2D fontAtlas;

in vec3 fColor;
in vec2 fUVs;

out vec4 Color;

void main(){
      Color = texture(fontAtlas, vec2(fUVs.x, 1.0 - fUVs.y)) * vec4(fColor, 1.0);
}