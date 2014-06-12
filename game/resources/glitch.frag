uniform sampler2D texture;
uniform vec2 inputSize;
uniform vec2 textureSize;
uniform float magtitude;
uniform float delta;

void main(void) {
	vec2 coord = gl_TexCoord[0].xy;
	float rx = 1.0/textureSize.x * magtitude * sin(coord.y * 50.0 + delta) * clamp(tan(coord.y * 70.0 + delta), -2.0, 2.0) * sin(coord.y * 150.0 + delta);
	float gx = 1.0/textureSize.x * magtitude * sin(coord.y * 5.0 + delta) * clamp(tan(coord.y * 170.0 + delta), -2.0, 2.0) * sin(coord.y * 110.0 + delta);
	float bx = 1.0/textureSize.x * magtitude * sin(coord.y * 20.0 + delta) * clamp(tan(coord.y * 95.0 + delta), -2.0, 2.0) * sin(coord.y * 120.0 + delta);
	
	gl_FragColor.r = texture2D(texture, coord + vec2(rx, 0)).r;
	gl_FragColor.g = texture2D(texture, coord + vec2(gx, 0)).g;
	gl_FragColor.b = texture2D(texture, coord + vec2(bx, 0)).b;
}
