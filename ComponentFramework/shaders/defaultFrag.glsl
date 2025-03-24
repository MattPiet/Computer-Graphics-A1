#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 fragColor;
layout(location = 1) in vec2 textureCords;
uniform vec4 colour;
uniform sampler2D myTexture; 

void main() {
//vec4 kt = texture(myTexture,textureCords);
//    fragColor = vec4(1.0,0.4,0.4,0.0);
fragColor = colour;
}