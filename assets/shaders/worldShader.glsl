#type vertex
#version 410 core

layout (location=0) in vec2 aPos;
layout (location=1) in float aTexID;

out float fTexID;

uniform mat4 ortho;
uniform mat4 view;

void main(){
    fTexID = atexID;
    gl_Position = ortho * view * vec4(aPos.x, aPos.y, 0.0, 1.0);
}

#type fragment
#version 410 core

uniform sampler2D texture_atlas;

uniform vec2 sprite_dims;
uniform float cols;

in float fTexID;
out vec4 fColor;

void main(){
       //integer division
       index_2D = vec2(fTexID / cols, ftexID % cols);
       index_2D.x *= sprite_dims.x;
       index_2D.y *= sprite_dims.y;

       size = 
       fColor = texture(texture_atlas, index_2D / size)



       
}