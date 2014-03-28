uniform sampler2D texture;
uniform float value;

void main()
{
    float factor = (1.0 - value) * 320.0;
	vec2 pos = floor(gl_TexCoord[0].xy * factor + 0.5) / factor;
	gl_FragColor = texture2D(texture, pos) * gl_Color;
}
