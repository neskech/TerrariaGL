#type vertex
#version 410 core

layout (location=0) in vec2 aPos;
layout (location=1) in vec3 aColor;

out vec3 fColor;

uniform mat4 ortho;
uniform mat4 view;

void main(){
    fColor = aColor;
    gl_Position = ortho * view * vec4(aPos.x, aPos.y, 5.0, 1.0);
}

#type fragment
#version 410 core

in vec3 fColor;
out vec4 Color;

void main(){
 
      Color = vec4(fColor, 1.0);
}