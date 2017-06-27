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

vec2 skew (vec2 st) {
    vec2 r = vec2(0.0);
    r.x = 1.125*st.x;
    r.y = st.y+0.5*r.x;
    return r;
}

void mainImage( in vec2 f )
{
    float	t = iGlobalTime;
	vec2 R = iResolution.xy,
          uv  = vec2(f+R) / R.y;
    // 11 par colones
    // 3 colonnes
    //uv*=vec2(3.,11.);
    // X : [-1.5, 1.5]
    // Y : [-5.5, 5.5]
    mat2	rot = mat2(
    cos(t),
        sin(t),
        -sin(t),
        cos(t)
    
    );
    // step(t+mod(), 1)
    
	vec4	teal = vec4(0.3,.7,.9,1.0); // teal
    
    vec4	orange = vec4(0.95,.5,.1,1.0); //orange
    vec2	old;
    old = uv;
    uv*=10.;
//    old *= rot;
    //uv = fract(uv)-.5;
    vec2	a = vec2(-1.0,0.);
    vec2	b = vec2(1.,1.);
    vec2	c = vec2(1.,-1.);
//        uv *= rot;
    uv = skew(uv);
	uv = fract(uv);//-.50;
    uv*=.5;
    float	r1 = .125*sin(t+0.+(old.x+old.y) );//.250;
    float	r2 = .125*sin(t+1.57+(old.x+old.y) );//.1250;
    fragColor =  teal*(step(uv.y, uv.x) * step(.0+r2, uv.y) );//step( tri(a,b,c, uv), .00001);
    fragColor +=  orange*(step(uv.x, uv.y) * step(.0+r1, uv.x) );//step( tri(a,b,c, uv), .00001);
    
    
}
