#type vertex
#version 410 core

layout (location=0) in vec2 aPos;
layout (location=1) in vec2 auv;
layout (location=2) in float texID;
layout (location=3) in mat4 model;

out vec2 uv;
out float ftexID;

uniform mat4 ortho;
uniform mat4 view;

void main(){
    uv = auv;
    ftexID = texID;
    gl_Position = ortho * view * model * vec4(aPos.x, aPos.y, 0.0, 1.0);
}

#type fragment
#version 410 core

uniform sampler2D uTextures[8];

in vec2 uv;
in float ftexID;
out vec4 Color;

void main(){
       Color = texture(uTextures[int(ftexID)], uv);


       
}