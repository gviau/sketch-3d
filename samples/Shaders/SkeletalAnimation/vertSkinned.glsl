#version 330

uniform mat4 modelViewProjection;
uniform mat4 boneTransformationMatrices[64];

layout (location=0) in vec3 in_vertex;
layout (location=1) in vec3 in_normal;
layout (location=2) in vec2 in_uv;
layout (location=3) in vec4 in_bones;
layout (location=4) in vec4 in_weights;

out vec3 normal;
out vec2 uv;

void main() {
	mat4 boneTransform = boneTransformationMatrices[int(in_bones.x)] * in_weights.x +
						 boneTransformationMatrices[int(in_bones.y)] * in_weights.y +
						 boneTransformationMatrices[int(in_bones.z)] * in_weights.z +
						 boneTransformationMatrices[int(in_bones.w)] * in_weights.w;

	vec4 pos = boneTransform * vec4(in_vertex, 1.0);

	uv = in_uv;
	gl_Position = modelViewProjection * pos;
}