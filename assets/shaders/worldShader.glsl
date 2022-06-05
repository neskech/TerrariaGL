#type vertex
#version 410 core

layout (location=0) in vec2 aPos;
layout (location=1) in vec2 aUVS;

out vec2 fUVS;

uniform mat4 ortho;
uniform mat4 view;

void main(){
    fUVS = aUVS;
    gl_Position = ortho * view * vec4(aPos.x, aPos.y, 0.0, 1.0);
}

#type fragment
#version 410 core

uniform sampler2D texture_atlas;

uniform vec2 sprite_dims;
uniform float cols;

in vec2 fUVS;
out vec4 fColor;

void main(){
 
      fColor = texture(texture_atlas, fUVS);

       
}