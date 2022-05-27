#type vertex
#version 410 core

layout (location=0) in vec3 aPos;
layout (location=1) in vec3 aColor;

out vec4 fColor;

void main(){
    fColor = vec4(aColor, 1.0);
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}

#type fragment
#version 410 core

in vec4 fColor;
out vec4 Color;

void main(){
       Color = fColor;
}