#version 330

uniform sampler2D texture0;

uniform vec3 light_diffuse;
uniform vec4 light_specular;

in vec3 normal;
in vec3 pos;
in vec2 uv;

out vec4 position;
out vec4 norm;
out vec4 albedo; 

void main() {
	position = pos;
	norm = normal;
	albedo = texture2D(texture0, uv).xyz;
}