#version 330

uniform sampler2D hdrTexture;

in vec2 uv;

out vec4 color;

vec3 GetLDR(vec3 hdr) {
	if (hdr.x > 1.0 || hdr.y > 1.0 || hdr.z > 1.0) {
		return vec3(0.0);
	}

	return hdr;
}

vec3 SuppressLDR(vec3 hdr) {
	if (hdr.x > 1.0 || hdr.y > 1.0 || hdr.z > 1.0) {
		return hdr;
	}

	return vec3(0.0);
}

void main() {
	vec3 hdr = texture(hdrTexture, uv).rgb;
	vec3 ldr = GetLDR(hdr);
	//hdr = SuppressLDR(hdr);
	color.rgb = pow( hdr / (hdr + 1.0), vec3(1.0 / 2.2) );
	//color.rgb = hdr / (hdr + 1.0);
	color.a = 1.0;
}