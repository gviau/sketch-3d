#version 420

in vec3 normal;

out vec4 color;

void main()
{
	color = vec4(normal, 1.0);
}