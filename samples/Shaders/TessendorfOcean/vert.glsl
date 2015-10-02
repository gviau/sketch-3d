#version 420

layout(binding = 0) uniform DrawConstants_t
{
    mat4x4 modelMatrix;

    mat4x4 modelViewProjectionMatrix;
    mat4x4 ViewProjectionMatrix;
    mat4x4 modelViewMatrix;

    mat4x4 inverseTransposeModelViewMatrix;
};

layout(binding = 1) uniform OceanConstants_t
{
    vec4 light_position;
    vec4 ambient_color;
    vec4 diffuse_color;
    vec4 specular_color;

    // x = diffuse, y = specular, z = ambient
    vec4 light_contribution;
};

layout (location=0) in vec3 in_vertex;
layout (location=1) in vec3 in_normal;

out vec3 normal;
out vec3 light_dir;
out vec3 eye;

void main() {
	vec3 pos = (modelViewMatrix * vec4(in_vertex, 1.0)).xyz;

	normal = normalize(mat3(modelViewMatrix) * in_normal);
	light_dir = normalize(( mat3(modelViewMatrix) * light_position.xyz) - pos);
	eye = normalize(-pos);

	gl_Position = modelViewProjectionMatrix * vec4(in_vertex, 1.0);
}