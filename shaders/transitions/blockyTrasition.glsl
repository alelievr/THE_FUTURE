
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

	if(sin(iGlobalTime) > rand(lowresxy)){
		fragColor = vec4(uv,0.5+0.5*sin(5.0 * fragCoord.x),1.0);
		//fragColor = vec4(uv,0.5+0.5*sin(iGlobalTime * 5.0 * uv.x),1.0);
		//fragColor = vec4(uv,0.5+0.5*sin(iGlobalTime * 5.0 * uv.x * sin(uv.x * uv.y * iGlobalTime)),1.0);
		//fragColor = vec4(uv,0.5+0.5*sin(iGlobalTime * 5.0 * uv.x * sin(uv.x / uv.y * iGlobalTime / 500000.0)),1.0);
		//fragColor = vec4(uv,0.5+0.5*sin(iGlobalTime * 5.0 * uv.x * sin(uv.x * uv.y * iGlobalTime / 5000.0)),1.0);
		//fragColor = vec4(uv,0.5+0.5*sin(iGlobalTime * 500.0 * uv.x * sin(uv.y + iGlobalTime * 500.0)),1.0);
		//fragColor = vec4(uv,0.5+0.5*sin((uv.x + uv.y) * 500.0 * sin(iGlobalTime)),1.0);
	}else{
		fragColor = vec4(0.0,0.0,0.0,1.0);
	}
}
