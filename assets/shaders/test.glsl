#type vertex
#version 330 core

layout (location=0) in vec3 aPos;
layout (location=1) in vec3 aColor;

out vec4 fColor;

void main(){
    fColor = aColor;
    gl_Position =  vec3(aPos, 1.0f);
}

#type fragment
#version 330 core

in vec3 fColor;
out vec4 Color;

void main(){
       Color = vec4(fColor, 1.0);
}