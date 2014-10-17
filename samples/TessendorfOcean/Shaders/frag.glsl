#version 330

uniform vec3 ambient_color;
uniform vec3 diffuse_color;
uniform vec4 specular_color;

// x = diffuse, y = specular, z = ambient
uniform vec3 light_contribution;

in vec3 normal;
in vec3 light_dir;
in vec3 eye;

out vec4 color;

void main() {
	vec3 N = normalize(normal);
	vec3 L = normalize(light_dir);
	vec3 V = normalize(eye);
	vec3 H = normalize(V + L);

	float lambert = dot(N, L);
	bool facing = lambert > 0.0;

	color.rgb = ambient_color * light_contribution.z +
				diffuse_color * light_contribution.x * max(lambert, 0.0) +
				(facing ? specular_color.xyz * light_contribution.y * max(pow(dot(N, H), specular_color.w), 0.0)
						: vec3(0.0));
	color.a = 1.0;

	color.rgb = pow(color.rgb, vec3(1.0/2.2));
	//color = vec4(light, 1.0);
}