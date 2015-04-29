#version 330

layout (location=0) in vec3 in_vertex;

out vec2 uv;

vec2 pixelOffset = vec2( 0.001953195, 0.00260416666 );

void main() {
	uv = in_vertex.xy * 0.5 + 0.5 + pixelOffset * 0.5;
	gl_Position = vec4(in_vertex.xy, 0.0, 1.0);
}