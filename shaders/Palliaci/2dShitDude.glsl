#pragma iChannel0 textures/lips.jpg

#define BAND(u) (ceil(u*4.)*.25)

#define sin(t) (.5*sin(t) )
#define PI 3.14159265358
float	t;
vec3 p0(vec2 u, float a)
{
	vec3	ret;
    
    ret.xyz = vec3( abs(2.*sin( ceil( (-a+u.y)*32.))),
                    abs(2.*sin( ceil( (-a+u.y)*32.+1.04))),
                    abs(2.*sin( ceil( (-a+u.y)*32.+2.08))));
    // fancy stuff with ret
    
    return ret;
}

vec2 skew (vec2 st) {
    vec2 r = vec2(0.0);
    r.x = 1.125*st.x;
    r.y = st.y+0.5*r.x;
    return r;
}

vec4	teal = vec4(0.3,.7,.9,1.0); // teal
    
    vec4	orange = vec4(0.95,.5,.1,1.0); //orange

vec3 p1(vec2 u, float a)
{
	vec3	ret;
    u-=vec2(.08,a);
    u = fract(skew(u*10.))+.1*(sin(u.x*t*4.)+.11);
    
//    ret+=1.;
    ret+= teal.xyz*step(u.x,u.y)*step(0., u.y);
    ret = max(ret, orange.xyz*step(u.y,u.x)*step(0., u.x) );
    // fancy stuff with ret
    
    return ret;
}
vec3 p2(vec2 u, float a)
{
	vec3	ret;
    u-=vec2(0.,a);
	u = abs(u)-.5;
//    u = mod(u, .125);
    u *= mat2(cos(t),sin(t),-sin(t),cos(t));
    u = abs(u);
    ret = vec3( float(int(u.x*100.)&int(u.y*100.) )*.06,
                float(int(u.x*100.)&int(u.y*100.) )*.03,
                float(int(u.x*100.)&int(u.y*100.) )*.05
              );
    
    // fancy stuff with ret
    
    return ret;
}
vec3 p3(vec2 u, float a)
{
	vec3	ret;
    
    u-=vec2(0.,a);
	//u = abs(u)-.5;
    
    ret.xyz = vec3(step(.00001,dot(u-.5+.3,u-.5)) );

    
    return ret;
}
vec3 p4(vec2 u, float a)
{
	vec3	ret;
    ret.xyz = vec3( abs(2.*sin( ceil( (-a+u.y)*32.)*sin(u.x*t*2.))),
                    abs(2.*sin( ceil( (-a+u.y)*32.+1.04)*sin(u.x*t*2.))),
                    abs(2.*sin( ceil( (-a+u.y)*32.+2.08))));
    
    return ret;
}
vec3 p5(vec2 u, float a)
{
	vec3	ret;
    
    // fancy stuff with ret
    
    return ret;
}
vec3 p6(vec2 u, float a)
{
	vec3	ret;
    
    // fancy stuff with ret
    
    return ret;
}
vec3 p7(vec2 u, float a)
{
	vec3	ret;
    u-=vec2(0.,a);
    u = fract(skew(u*4.));
    
    vec2 pos = vec2(0.7)-u;

    float r = length(pos)*2.0;
    float ff = sin(u.x);
   	ret = vec3(smoothstep(ff,ff+0.02,r));
    for (float i = 1.; i <= 15. ; i++)
    {
    	ret += sin(i)*vec3(smoothstep(ff,ff+0.02,r*.5*i));
    }

/*
    float r = length(sin(pos))*2.0;
    float ff = sin(u.x);
    ret.y = 1.-smoothstep(ff,ff+0.02,r);*/
    return ret;
}
vec3 p8(vec2 u, float a)
{
	vec3	ret;
    
    // fancy stuff with ret
    
    return ret;
}
vec3 p9(vec2 u, float a)
{
	vec3	ret;
    
    // fancy stuff with ret
    
    return ret;
}
vec3 p10(vec2 u, float a)
{
	vec3	ret;
    
    // fancy stuff with ret
    
    return ret;
}
vec3 p11(vec2 u, float a)
{
	vec3	ret;
    
    ret = texture(iChannel0, u).xyz;
    
    return ret;
}

void mainImage( in vec2 fragCoord )
{
    t = iGlobalTime;
	vec2 u = vec2(.95, 1.)*fragCoord.xy / iResolution.xy;
	vec4 o;

    
//    o = ceil(u.x*4.)*vec4(.25);

    float	a, b, c, d;
    a = .0, b = .08, c= .1, d = .7;
    c = .1+ sin(t+2.08);
    d = .7 + sin(t+2.08);

    for(float i = 0.; i <= 11.; i++)
    {
   		c = .0 + sin(t+( floor(i)/3. )*PI/3. )+.1*(sin(u.x*t*4.)+.11);
	    d = .8 + sin(t+( floor(i)/3. )*PI/3. );
        if( i == 0.)
	    o.xyz = max(o.xyz, p0(u, c)*step(a, (u.x) )*step( (u.x), b)*step(c, u.y)*step(u.y, d) );
        if( i == 1.)
	    o.xyz = max(o.xyz, p1(u, c)*step(a, (u.x) )*step( (u.x), b)*step(c, u.y)*step(u.y, d) );
        if( i == 2.)
	    o.xyz = max(o.xyz, p2(u, c)*step(a, (u.x) )*step( (u.x), b)*step(c, u.y)*step(u.y, d) );
        if( i == 3.)
	    o.xyz = max(o.xyz, p3(u, c)*step(a, (u.x) )*step( (u.x), b)*step(c, u.y)*step(u.y, d) );
        if( i == 4.)
	    o.xyz = max(o.xyz, p4(u, c)*step(a, (u.x) )*step( (u.x), b)*step(c, u.y)*step(u.y, d) );
        if( i == 5.)
	    o.xyz = max(o.xyz, p5(u, c)*step(a, (u.x) )*step( (u.x), b)*step(c, u.y)*step(u.y, d) );
        if( i == 6.)
	    o.xyz = max(o.xyz, p6(u, c)*step(a, (u.x) )*step( (u.x), b)*step(c, u.y)*step(u.y, d) );
        if( i == 7.)
	    o.xyz = max(o.xyz, p7(u, c)*step(a, (u.x) )*step( (u.x), b)*step(c, u.y)*step(u.y, d) );
        if( i == 8.)
	    o.xyz = max(o.xyz, p8(u, c)*step(a, (u.x) )*step( (u.x), b)*step(c, u.y)*step(u.y, d) );
        if( i == 9.)
	    o.xyz = max(o.xyz, p9(u, c)*step(a, (u.x) )*step( (u.x), b)*step(c, u.y)*step(u.y, d) );
        if( i == 10.)
	    o.xyz = max(o.xyz, p10(u, c)*step(a, (u.x) )*step( (u.x), b)*step(c, u.y)*step(u.y, d) );
        if( i == 11.)
	    o.xyz = max(o.xyz, p11(u, c)*step(a, (u.x) )*step( (u.x), b)*step(c, u.y)*step(u.y, d) );
        
        a += .08;b+=.08;
    }
//	fragColor = texture(iChannel0, u+.1*(sin(u.x*t*4.)+.11));
	fragColor = texture(iChannel0, u);
	fragColor.xyz *= o.xyz;
	fragColor.w  = 1.;

    //o = vec4(uv,0.5+0.5*sin(iGlobalTime),1.0);
}
