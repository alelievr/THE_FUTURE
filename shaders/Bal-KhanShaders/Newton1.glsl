#define I_MAX 12
float	t;
vec2 	cmult(vec2 a, vec2 b);
vec2	cadd(vec2 a, vec2 b);

vec2 g(vec2 n) {
    return vec2(
        n.x*n.x*n.x - 3.*n.x*n.y*n.y - 100.,
        -n.y*n.y*n.y + 3.*n.x*n.x*n.y
    );
}
vec2 dg(vec2 n) {
    return 2. * vec2(
        n.x*n.x - n.y*n.y,
        2. * n.x * n.y
    );
}
vec2 cdiv(vec2 a, vec2 b) {
    float d = dot(b, b);
    if(d == 0.) return a;
    else return vec2(
        (a.x*b.x + a.y*b.y),
        (a.y*b.x - a.x*b.y)) / d;;
}

void	mainImage( in vec2 fragCoord )
{
    t = iGlobalTime*.25;
    vec2	uv = fragCoord.xy / iResolution.xy;
    vec4	z = vec4(0.0, 0.0, 0.0, 0.0);
    vec2	of = 2.*vec2( (uv.x -.5)/0.125, (uv.y -.5)/0.125);
    vec3	col = vec3(0.0);
    vec2	dist = vec2(0.0);
    z.xy = of;
    int ii = -1;
    float  param_i = .0;
    for (int i = -1; i < I_MAX; ++i)
    {
        ++ii;
        z.xy = z.xy-cdiv(g(z.xy+of-2.*vec2(cos(1.*t/5.0+float(ii)*param_i ),sin(1.*t/3.0+float(ii)*param_i ))),
                         dg(z.xy+of-2.*vec2(cos(1.*t/2.0+float(ii)*param_i ),sin(1.*t/1.0+float(ii)*param_i ))) );
        z.xy = cmult(z.xy, vec2(sin(t), cos(t))).xy;
        //z.xy = abs(z.xy)-5.;//-.01*float(ii);
        z.z = 2.0 * (z.x*z.z - z.y*z.w);
        z.w = 2.0 * (z.y*z.z - z.x*z.w);
        dist.x = dot(z.xy,z.xy);
	       dist.y = dot(z.zw,z.zw);
        if ( float(ii) > 0.
            &&
            (
             sqrt(z.x*z.x/(z.y*z.y) ) < .051
            ||
             sqrt(z.y*z.y/(z.x*z.x) ) < .051
                )
            //sqrt(z.x*z.x*(z.y*z.y) ) < .54321
            //sqrt(z.x*z.x/(z.y*z.y) ) < .1
//           &&
  //           sqrt(z.x*z.z+z.y*z.y) >= .05
           )
        {
        col.x = exp(-abs(z.x*z.x*z.y))*1.; // expensive but pretty
    	col.y = exp(-abs(z.y*z.x*z.y))*1.;
    	col.z = exp(-abs(min(z.x,z.y) ))*1.;
       	//col.xyz += vec3(100.);
            break;
        }

     	col.x += .1*exp(-abs(-z.x/float(ii)+float(ii)/z.x))*2.; // expensive but pretty
    	col.y += .1*exp(-abs(-z.y/float(ii)+float(ii)/z.y))*2.;
    	col.z += .1*exp(-abs((-z.x-z.y)/float(ii)+float(ii)/(z.x+z.y)))*2.;
        if (dist.x > 1000000000000000.0)// || dist.y > 100000000000.0)
            break;
    }
    //col /= vec3(float(ii));
    fragColor = 1.*vec4(col, 1.0);
}

vec2	cmult(vec2 a, vec2 b)
{
    return (vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x));
}

vec2	cadd(vec2 a, vec2 b)
{
	return (vec2(a.x + b.x, a.y + b.y));
}
