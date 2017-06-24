
float rand(vec2 co){
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void mainImage( in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy / iResolution.xy;
	float resolution = 5.0;
	vec2 lowresxy = vec2(
		    floor(fragCoord.x / resolution),
		    floor(fragCoord.y / resolution)
		    );

	float t = iGlobalTime * 3;
	if (t < 1)
	{
		if(sin(t) > rand(lowresxy))
			fragColor = vec4(0, 0, 0, 1.0);
		else
			fragColor = vec4(0);
	}
	else
	{
		if(sin(t) > rand(lowresxy))
			fragColor = vec4(0, 0, 0, 1.0);
		else
			fragColor = vec4(0);
	}
}
