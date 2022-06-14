#type vertex
#version 410 core

layout (location=0) in vec2 aPos;
layout (location=1) in float aUVs;
layout (location=2) in vec2 texID;
layout (location=3) in vec4 aColor;
layout (location=4) in mat4 model;

out vec2 fUVs;
out float ftexID;
out vec4 fColor;

uniform mat4 ortho;
uniform mat4 view;

void main(){
    fUVs = aUVs;
    ftexID = texID;
    fColor = aColor;
    gl_Position = ortho * view * model * vec4(aPos.x, aPos.y, 0.0, 1.0);
}

#type fragment
#version 410 core

uniform sampler2D uTextures[8];

in vec2 fUVs;
in float ftexID;
in vec4 fColor;

out vec4 Color;

void main(){
       Color = texture(uTextures[int(ftexID)], fUVs) * fColor;
       
}