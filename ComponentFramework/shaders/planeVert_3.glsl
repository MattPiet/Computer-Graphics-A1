#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 uvCoord;

uniform mat4 planeModelMatrix_3;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;


layout(location = 0) out vec3 vertNormal;
layout(location = 1) out vec3 lightDir[5]; 
layout(location = 6) out vec3 eyeDir; 
layout(location = 7) out vec2 textureCoords; 


 uniform  vec3 lightPosition[5];  // Array of 5 light positions


void main() {
    textureCoords = uvCoord;
    mat3 normalMatrix = mat3(transpose(inverse(planeModelMatrix_3)));
    vertNormal = normalize(normalMatrix * vNormal); /// Rotate the normal to the correct orientation 
    vec3 vertPos = vec3(viewMatrix * planeModelMatrix_3 * vVertex);
    vec3 vertDir = normalize(vertPos);
    eyeDir = -vertDir;
   for(int i = 0; i < 5; i++){
    lightDir[i] = normalize(vec3(lightPosition[i]) - vertPos); 
    }
    gl_Position = projectionMatrix * viewMatrix * planeModelMatrix_3 * vVertex;
}
