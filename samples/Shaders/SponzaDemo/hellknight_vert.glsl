#version 330

uniform mat4 modelViewProjection;
uniform mat4 modelView;
uniform mat4 boneTransformationMatrices[100];

layout (location=0) in vec3 in_vertex;
layout (location=1) in vec3 in_normal;
layout (location=2) in vec2 in_uv;
layout (location=3) in vec3 in_tangent;
layout (location=4) in vec4 in_bones;
layout (location=5) in vec4 in_weights;

out vec3 normal;
out vec2 uv;
out vec3 tangent;

void main() {
	mat4 boneTransformation = boneTransformationMatrices[int(in_bones.x)] * in_weights.x +
							  boneTransformationMatrices[int(in_bones.y)] * in_weights.y +
							  boneTransformationMatrices[int(in_bones.z)] * in_weights.z +
							  boneTransformationMatrices[int(in_bones.w)] * in_weights.w;
	vec4 pos = boneTransformation * vec4(in_vertex, 1.0);

	uv = in_uv;
	normal = mat3(modelView) * (boneTransformation * vec4(in_normal, 0.0)).xyz;
	tangent = mat3(modelView) * (boneTransformation * vec4(in_tangent, 0.0)).xyz;

	gl_Position = modelViewProjection * pos;
}