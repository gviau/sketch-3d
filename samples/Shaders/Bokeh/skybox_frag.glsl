#version 330

in vec3 eye_direction;

out vec4 color;

// From MJP's DX11 Sample Framework
//  http://mynameismjp.wordpress.com/
float AngleBetween(vec3 dir0, vec3 dir1) {
	return acos(dot(dir0, dir1));
}

void main() {
	vec3 dir = normalize(eye_direction);
	vec3 sun_direction = vec3(0.577, 0.577, 0.577);
	float gamma = AngleBetween(dir, sun_direction);
	float S = AngleBetween(sun_direction, vec3(0.0, 1.0, 0.0));
	float theta = AngleBetween(dir, vec3(0.0, 1.0, 0.0));

	float cosTheta = cos(theta);
	float cosS = cos(S);
	float cosGamma = cos(gamma);

	float num = (0.91 + 10 * exp(-3 * gamma) + 0.45 * cosGamma * cosGamma) * (1.0 - exp(-0.32 / cosTheta));
	float denom = (0.931 + 10 * exp(-3 * S) + 0.45 * cosS * cosS) * (1.0 - exp(-0.32));

	float lum = num / denom;
	vec3 skyColor = vec3(0.2, 0.5, 1.0);
	vec3 sunColor = vec3(1.0, 0.8, 0.3) * 150;
	float sunWidth = 0.05;

	color.rgb = mix(sunColor, skyColor, clamp(abs(gamma) / sunWidth, 0.0, 1.0) );
	color = vec4(max(color.rgb * lum, vec3(0.0, 0.0, 0.0)), 1.0);
}