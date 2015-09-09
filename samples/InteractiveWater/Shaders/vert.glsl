#version 420

layout(binding = 0) uniform DrawConstants_t
{
    mat4x4 modelMatrix;

    mat4x4 modelViewProjectionMatrix;
    mat4x4 ViewProjectionMatrix;
    mat4x4 modelViewMatrix;

    mat4x4 inverseTransposeModelViewMatrix;
};

layout (location=0) in vec3 in_position;
layout (location=1) in vec2 in_uv;
layout (location=2) in vec3 in_normal;

out vec3 normal;

void main()
{
	gl_Position = modelMatrix * vec4(in_position, 1.0);
	normal = in_normal;
}