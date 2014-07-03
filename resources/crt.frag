uniform sampler2D texture;
uniform vec2 inputSize;
uniform vec2 textureSize;

#define distortion 0.2

vec2 barrelDistortion(vec2 coord) {
	vec2 cc = coord - 0.5;
	float dist = dot(cc, cc);
	return coord + cc * (dist + distortion * dist * dist) * distortion;
}

void main(void) {
	vec2 coord = barrelDistortion(gl_TexCoord[0].xy * textureSize / inputSize) * inputSize / textureSize;
	coord = ((coord - 0.5 * inputSize / textureSize) * (1.0 - (0.5 + distortion * 0.5 * 0.5) * distortion)) + 0.5 * inputSize / textureSize;
	gl_FragColor = texture2D(texture, coord);
	gl_FragColor *= 0.8 + (abs(fract(coord.y * inputSize.y * 0.35) - 0.5)) * 2.0 * 0.4;
}
