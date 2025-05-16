#version 450
#extension GL_ARB_separate_shader_objects : enable


layout(location = 0) in vec3 inVertex;
layout(location = 1) in vec3 inNormal;

layout(location = 0) uniform mat4 projectionMatrix;
layout(location = 1) uniform mat4 viewMatrix;
layout(location = 2) uniform mat4 modelMatrix;
layout(location = 3) uniform float time;

out vec3 Rotated_normal;
out vec3 vertexPosition;


void main() {

    mat3 rotation_matrix = mat3(transpose(inverse(modelMatrix)));

    Rotated_normal = rotation_matrix * inNormal;

    vertexPosition = vec3(modelMatrix * vec4(inVertex, 1.0));

    float amplitude = 0.5f;
    float frequency = 2.0f;
    float phaseAngle = vertexPosition.y;
    vec3 displacement;
    displacement.x = 0;
    displacement.y = amplitude * sin(frequency * time + phaseAngle);
    displacement.z = 0;

    vertexPosition += displacement;

    
    gl_Position = projectionMatrix * viewMatrix * vec4(vertexPosition, 1.0);
}