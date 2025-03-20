#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 fragColor;

in vec3 Rotated_normal;
in vec3 vertexPosition;

uniform samplerCube skybox;
uniform vec3 cameraPos;

void main() {
  float ratio = 1.00 / 1.52;
    vec3 view_direction_vector = normalize(vertexPosition - cameraPos);
    vec3 reflection_direction_vector = reflect(view_direction_vector, normalize(Rotated_normal));

    vec3 Refraction = refract(view_direction_vector, normalize(Rotated_normal), ratio);

 
    float cosTheta = dot(-view_direction_vector, normalize(Rotated_normal));
    vec3 reflectedColor = texture(skybox, reflection_direction_vector).rgb;
    vec3 refractedColor = texture(skybox, Refraction).rgb;

   // vec3 finalColor = mix(refractedColor, reflectedColor,cosTheta);
    vec3 finalColor = mix(reflectedColor,refractedColor ,cosTheta);
    fragColor = vec4(finalColor, 1.0);


}
