#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inVertex;
layout(location = 1) in vec3 inNormal;

layout(location = 0) uniform mat4 projectionMatrix;
layout(location = 1) uniform mat4 viewMatrix;
layout(location = 2) uniform mat4 modelMatrix;

out vec3 Rotated_normal;
out vec3 vertexPosition;


void main() {
                //InvTran is the rotation matrix
    Rotated_normal = mat3(transpose(inverse(modelMatrix))) * inNormal;
    vertexPosition = vec3(modelMatrix * vec4(inVertex, 1.0));
    gl_Position = projectionMatrix * viewMatrix * vec4(vertexPosition, 1.0);
}


