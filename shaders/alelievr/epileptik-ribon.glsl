#define MAGIC(x,y) fract(dot(x*2., y*2.)*1.)

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	float gTime = iGlobalTime+11.0;
	float f = 0., g = 0., h = 0.;
	vec2 res = iResolution.xy;
	vec2 mou;
	mou.x = sin(gTime * .05)/2.-.5;
	mou.y = 0.0;
	mou = (mou+1.0) * res;
	vec2 z = ((-res+2.0 * fragCoord.xy) / res.y);
	vec2 p = ((-res+2.0+mou) / res.y);
	//  z.x = abs(z.x);
	for( int i = 0; i < 30; i++)
	{
	    float d = dot(z,z);
	    z = (vec2( z.x, -z.y ) / d) + p;
	    z.x = abs(z.x);
	    f = max(f, MAGIC(z-p,z-p));
	    g = max(g, MAGIC(z+p,z-p));
	    h = max(h, MAGIC(p-z,p+z));
	}
	vec3 col = ((vec3(f, g, h) - 0.875)*8.);
	fragColor = vec4(col, 1.0);
}

