#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (triangles, equal_spacing, ccw) in;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
//uniform sampler2D textureData;
uniform float height_scale;
uniform vec3 lightPosition;


layout(binding = 0) uniform sampler2D terrain_height_map;
layout(binding = 1) uniform sampler2D terrain_normal_map;


in vec2 uvCoordFromCtrl[];
in vec3 normalFromCtrl[];

out vec2 uvCoordFromEval;
out vec3 normalFromEval;

out vec3 eye_dir_from_eval;
out vec3 light_Dir_from_eval;

out float fog_val_from_eval;

vec2 interpolateVec2(vec2 v0, vec2 v1, vec2 v2) {
    return gl_TessCoord.x * v0 + gl_TessCoord.y * v1 + gl_TessCoord.z * v2;
}

vec3 interpolateVec3(vec3 v0, vec3 v1, vec3 v2) {
    return  gl_TessCoord.x * v0 + gl_TessCoord.y * v1 + gl_TessCoord.z * v2;
}

vec4 interpolateVec4(vec4 v0, vec4 v1, vec4 v2) {
    return  gl_TessCoord.x * v0 + gl_TessCoord.y * v1 + gl_TessCoord.z * v2;
}

const float fog_density = 0.025;
const float fog_gradiant = 1.5;

void main() {

    uvCoordFromEval = interpolateVec2(uvCoordFromCtrl[0], uvCoordFromCtrl[1], uvCoordFromCtrl[2]);
    normalFromEval =  interpolateVec3(normalFromCtrl[0], normalFromCtrl[1], normalFromCtrl[2]);
    /// Interpolate position across the triangle
    vec4 position = interpolateVec4(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position);

    vec4 height = vec4(texture(terrain_height_map,uvCoordFromEval));
    position.z =  height.r * height_scale;

    
    vec3 vertPos = vec3(viewMatrix * modelMatrix * position);
    vec3 vertDir = normalize(vertPos);
    eye_dir_from_eval = -vertDir;
   
    light_Dir_from_eval = normalize(lightPosition - vertPos); 
    
    float distance = length(vertPos);
    fog_val_from_eval = clamp(exp(-pow((distance*fog_density),fog_gradiant)), 0.0, 1.0);

    gl_Position =  projectionMatrix * viewMatrix * modelMatrix * (position );
}
