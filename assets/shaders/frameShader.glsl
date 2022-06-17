#type vertex
#version 410 core

layout (location=0) in vec2 aPos;
layout (location=1) in vec2 aUVs;

out vec2 fUVs;

void main(){
    fUVs = aUVs;
    gl_Position = vec4(aPos, 0.0, 1.0);
}

#type fragment
#version 410 core

uniform sampler2D screenTexture;

uniform float ambience;
uniform vec3 ambientColor;

uniform float numTorches;
uniform vec2[8] torchLocations;

in vec2 fUVs;
out vec4 Color;

vec3 calcPointLight(vec2 lightPos, vec2 fragPos){
    float distance = length(lightPos - fragPos);

    vec3 lightColor = vec3(0.7, 0.7, 0.4);

    float constant = 1.0;
    float linear = 0.0005;
    float quadratic = 0.00002;

    float attentuation = 1.0 / (constant + linear * distance + quadratic * distance * distance);

    vec3 diff = lightColor * vec3(texture(screenTexture, fUVs));

    diff *= attentuation;

    return diff;

}

void main(){
  
    vec3 outp = vec3(0.0);

    for (int i = 0; i < int(numTorches); i++){
        outp += calcPointLight(torchLocations[i], gl_FragCoord.xy);
    }
    outp += ambience * vec3(texture(screenTexture, fUVs)) + ambientColor * 0.07;

    Color = vec4(outp, 1.0);
}