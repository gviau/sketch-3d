#version 330

uniform sampler2D HDRTexture;
uniform sampler2D bluredHDR;

in vec2 uv;

out vec4 color;

// From http://frictionalgames.blogspot.ca/2012/09/tech-feature-hdr-lightning.html
vec3 Uncharted2Tonemap(vec3 x) {
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	return ((x * (A*x + C*B) + D*E) / (x * (A*x + B) + D*F)) - E/F;
}

void main() {
	vec3 hdr = texture(HDRTexture, uv).xyz;
	vec3 blur = texture(bluredHDR, uv).xyz;
	vec3 composedColor = hdr * 0.6 + blur * 0.4;
	
	//color.rgb = Uncharted2Tonemap(composedColor);

	//vec3 c = Uncharted2Tonemap(hdr * 2.0) / Uncharted2Tonemap(vec3(11.2));
	//color.rgb = pow( c, vec3(1.0 / 2.2) );
	//color.rgb = pow( hdr + blur, vec3(1.0 / 2.2) );
	color.rgb = pow( Uncharted2Tonemap(composedColor * 2.0), vec3(1.0 / 2.2) );
	color.a = 1.0;
}