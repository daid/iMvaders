uniform sampler2D texture;
uniform float value;

void main()
{
	float f = 1.0 / 320.0;
	vec4 i00   = texture2D(texture, gl_TexCoord[0].xy);
    vec4 im1m1 = texture2D(texture, gl_TexCoord[0].xy + vec2(-f,-f));
    vec4 ip1p1 = texture2D(texture, gl_TexCoord[0].xy + vec2( f, f));
    vec4 im1p1 = texture2D(texture, gl_TexCoord[0].xy + vec2(-f, f));
    vec4 ip1m1 = texture2D(texture, gl_TexCoord[0].xy + vec2( f,-f));
    vec4 im10 = texture2D(texture, gl_TexCoord[0].xy + vec2(-f, 0));
    vec4 ip10 = texture2D(texture, gl_TexCoord[0].xy + vec2( f, 0));
    vec4 i0m1 = texture2D(texture, gl_TexCoord[0].xy + vec2( 0,-f));
    vec4 i0p1 = texture2D(texture, gl_TexCoord[0].xy + vec2( 0, f));
    vec4 h = -im1p1 - 2.0 * i0p1 - ip1p1 + im1m1 + 2.0 * i0m1 + ip1m1;
    vec4 v = -im1m1 - 2.0 * im10 - im1p1 + ip1m1 + 2.0 * ip10 + ip1p1;

    gl_FragColor = vec4(length(vec2(h.r, v.r)) / 4.0, length(vec2(h.g, v.g)) / 4.0, length(vec2(h.b, v.b)) / 4.0, 1.0) * value + i00 * (1.0 - value);
}
