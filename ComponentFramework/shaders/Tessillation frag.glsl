#version 450
#extension GL_ARB_separate_shader_objects : enable
in vec2 uvCoordFromEval;
in vec3 normalFromEval;
out vec4 fragColor;
layout(binding = 1) uniform sampler2D terrain_normal_map;
layout(binding = 2) uniform sampler2D terrain_diffuse_map;

in vec3 eye_dir_from_eval;
in vec3 light_Dir_from_eval;
in float fog_val_from_eval;

void main() {
    vec4 deepColour = vec4(0.01,0.2,0.4,0.0);
    vec4 colour_diffuse = vec4(0.5,0.45,0.4,0.0);
    vec4 colour_specular = 0.7 * colour_diffuse;
    vec4 colour_ambient = 0.01 * colour_diffuse;
    vec4 kt = texture(terrain_diffuse_map,uvCoordFromEval);

    vec3 normal_from_map = texture(terrain_normal_map,uvCoordFromEval).xyz * 2.0 - 1.0;

    float diff = max(dot(light_Dir_from_eval, normal_from_map), 0.0);

	/// Reflection is based incedent which means a vector from the light source
	/// not the direction to the light source so flip the sign
	vec3 reflection = normalize(reflect(-light_Dir_from_eval, normal_from_map));
	
	float spec = max(dot(eye_dir_from_eval, reflection), 0.0);
	spec = pow(spec,14.0);
    vec4 colour = (colour_ambient + (diff * colour_diffuse) + (spec * colour_specular)) * kt;
		
	fragColor = mix(deepColour, colour, fog_val_from_eval);

    //fragColor = vec4(0.0,1.0,0.0,1.0);
}