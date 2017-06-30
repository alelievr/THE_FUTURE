#define MAGIC(x,y) fract(dot(x*2., y*2.)*1.)

void mainImage( in vec2 fragCoord )
{
	float gTime = iGlobalTime+11.0;
	float f = 0., g = 0., h = 0.;
	vec2 res = iResolution.xy;
	vec2 mou;
	float demoniColor = sin(iGlobalTime * 0.0723456) / 2 + .5;
	mou.x = mix(-1, 0.1, sin(gTime * 0.0888876) / 2 + .5);
	mou.y = sin(iGlobalTime * 0.0033321) * 1.8;
	mou = (mou+1.0) * res;
	vec2 z = ((-res+2.0 * fragCoord.xy) / res.y);
	vec2 p = ((-res+2.0+mou) / res.y);
	float nIter = 20 + cos(iGlobalTime * 0.0012345) * 10;
	//  z.x = abs(z.x);
	for( int i = 0; i < int(nIter); i++)
	{
	    float d = dot(z,z);
	    z = (vec2( z.x, -z.y ) / d) + p;
	    z.x = abs(z.x);
	    f = max(f, MAGIC(z-p,z-p));
	    g = max(g, MAGIC(z+p,z-p));
	    h = max(h, MAGIC(p-z,p+z));
	}
	vec3 col = ((vec3(f, g, h) - 0.875)*8.);
	fragColor = vec4(abs(col-demoniColor), 1.0);
}

