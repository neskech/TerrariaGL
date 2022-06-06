#type vertex
#version 410 core

layout (location=0) in vec2 aPos;
layout (location=1) in float atexCord;
layout (location=2) in float texID;
layout (location=3) in mat4 model;

out float ftexCord;
out float ftexID;
out vec2 offset;

uniform mat4 ortho;
uniform mat4 view;

void main(){
    const vec2 offsets[4] = vec2[4](
           vec2(0, 1), vec2(1, 1),
           vec2(0, 0), vec2(1, 0)
    );
    
    offset = offsets[gl_VertexID];
    ftexCord = atexCord;
    ftexID = texID;
    gl_Position = ortho * view * model * vec4(aPos.x, aPos.y, 0.0, 1.0);
}

#type fragment
#version 410 core

uniform sampler2D uTextures[8];
uniform vec2 spriteDimensions[8];
uniform float cols[8];

in float ftexCord;
in float ftexID;
in vec2 offset;
out vec4 Color;

void main(){


       int index = int(ftexID);
       float row = float( int(ftexCord) / int(cols[index]) + offset.y );
       float col = float( int(ftexCord) % int(cols[index]) + offset.x );

       ivec2 texSize = textureSize(uTextures[index], 0);
       vec2 uv = vec2(col * spriteDimensions[index].x, row * spriteDimensions[index].y);
       uv.x /= float(texSize.x);
       uv.y /= float(texSize.y);
       uv.y = 1.0 - uv.y;

       Color = texture(uTextures[index], uv);


       
}