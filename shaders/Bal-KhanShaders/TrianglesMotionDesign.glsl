float tri( in vec2 p0, in vec2 p1, in vec2 p2, in vec2 p )
{
	vec2 e0 = p1 - p0;
	vec2 e1 = p2 - p1;
	vec2 e2 = p0 - p2;

	vec2 v0 = p - p0;
	vec2 v1 = p - p1;
	vec2 v2 = p - p2;

	vec2 pq0 = v0 - e0*clamp( dot(v0,e0)/dot(e0,e0), 0.0, 1.0 );
	vec2 pq1 = v1 - e1*clamp( dot(v1,e1)/dot(e1,e1), 0.0, 1.0 );
	vec2 pq2 = v2 - e2*clamp( dot(v2,e2)/dot(e2,e2), 0.0, 1.0 );
    
    vec2 d = min( min( vec2( dot( pq0, pq0 ), v0.x*e0.y-v0.y*e0.x ),
                       vec2( dot( pq1, pq1 ), v1.x*e1.y-v1.y*e1.x )),
                       vec2( dot( pq2, pq2 ), v2.x*e2.y-v2.y*e2.x ));

		       return -sqrt(d.x)*sign(d.y);
}

void mainImage( out vec4 o, in vec2 f )
{
    float	t = iGlobalTime;
    o = vec4(0.,0.,0.,1.0);
    vec2 R = iResolution.xy,
          uv  = vec2(f-R/2.) / R.y;
    vec2  a,b,c;
//    uv -= .5;
    uv *= 9.;
    a = vec2(-.25,-.25);
    b = vec2(+.25,-.25);
    c = vec2(+.00,.25);
//    o.x = 1.-sign( tri(b, a, c, uv) );
    float an = 1.57+t*.0125;//+t*.251;
    mat2  rot = mat2(
        cos(an),
        sin(an),
        -sin(an),
        cos(an)
        );
    float	s = 0.;
    for(float i =0.; i < 20.;i++)
    {
//        uv.xy*= rot;

        uv.x-=cos(i*.31);
        uv.y-=sin(i*.31);
        
//    	uv.xy *=rot;
        a*=-rot;//+1.+sin(i*.31+t*.5);
        b*=rot;//+1.+sin(i*.31+t*.5);
        c*=sign(a.x*b.x)+rot;//+1.+sin(i*.31+t*.5);
        o.x = max(1.-sign( -.1*(sin(3.14*i/20.+t*1.))+tri(b, a, c, uv+vec2(.5, 3.) ) ), o.x);
        if (o.x>0.)
            break;
        s++;
    }
    
    o.xyz = 1.-o.xxx ;//* vec3(sin(3.14*s/20.+t) );
}
