#pragma iChannel0 textures/fire.jpg
#pragma iChannel1 textures/ice.jpg
#pragma iChannel2 textures/EXPLOSION.png

void	mainImage(vec2 coord)
{
	vec2	uv = coord / iResolution;

	uv.x *= iResolution.x / iResolution.y;

	vec4 tx1 = texture(iChannel0, uv);
	vec4 tx2 = texture(iChannel1, uv);
	vec4 tx3 = texture(iChannel2, uv);
	fragColor = vec4(mix(mix(tx1, tx2, sin(iGlobalTime / 2)), tx3, sin(iGlobalTime)).xyz, 1);
}
