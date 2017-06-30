#pragma iChannel0 shaders/test/renderbuffer.glsl

void		mainImage(vec2 fragCoord)
{
	vec2 uv = fragCoord / iResolution;

	vec4 tx = texture(iChannel0, uv);
	fragColor = vec4(tx.xyz * (1-length(uv - 0.5)*2), 1);
}
