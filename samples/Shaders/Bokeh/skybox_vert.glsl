#version 330

uniform mat4 projection;
uniform mat4 view;

layout (location = 0) in vec3 in_vertex;

out vec3 eye_direction;

// From http://gamedev.stackexchange.com/questions/60313/implementing-a-skybox-with-glsl-version-330
void main() {
	mat4 inverseProj = inverse(projection);
	mat3 inverseModelView = transpose(mat3(view));
	vec3 unprojected = (inverseProj * vec4(in_vertex, 1.0)).xyz;
	eye_direction = inverseModelView * unprojected;
	
	gl_Position = vec4(in_vertex.xy, 0.0, 1.0);
}