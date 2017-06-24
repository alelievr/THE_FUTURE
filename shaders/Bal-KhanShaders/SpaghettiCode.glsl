/*
* License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
*/

float	t;

#define I_MAX		100
#define E				0.001

// Toogles

//#define	SQUARE
//#define	FANCY

vec4		march(vec3 pos, vec3 dir);
vec3		camera(vec2 uv);
vec3		calcNormal(in vec3 pos, float e, vec3 dir);
vec2		rot(vec2 p, vec2 ang);
void		rotate(inout vec2 v, float angle);

vec3		base; // base color for the item, this is modified in the spaghetti distance estimator

void mainImage(in vec2 f)
{
    t = iGlobalTime;
    vec3	col = vec3(0., 0., 0.);
    vec2	uv  = vec2(.35+f.x/iResolution.x, f.y/iResolution.y);
    vec3	dir = camera(uv);
    vec3	pos = vec3(-5.0, 5.0, 60.0);


    vec4	inter = (march(pos, dir));

    if (inter.y == 1.)
    {
	vec3	v = pos+inter.w*dir;
        vec3	n = calcNormal(v, E, dir);
        vec3	ev = normalize(v - pos);
		vec3 ref_ev = reflect(ev, n);
        vec3	light_pos   = pos+vec3(0.0, 50.0, 70.0);
        light_pos.z+=80.*sin(t);
		vec3	light_color = vec3(.8, .3, .2)*2.;
        vec3	vl = normalize( (light_pos - v) );
		float		diffuse  = max(0., dot(vl, n));
				float	 specular = pow(max(0., dot(vl, ref_ev)), 40. );
        #ifndef			SQUARE
        base.xyz = texture(iChannel0, vec2( (v.zz)*.042)).xyz;
        #else
        base.xyz = 1.5*texture(iChannel0, vec2( (base.xy)*1.)).xyz;
        #endif
        col.xyz = light_color * (specular) + diffuse * base;
        #ifdef	SQUARE
        col.xyz -= 1.5*vec3( inter.w / 200. ); // add more fake light effect (AO distance based)
        #endif
    }
    fragColor =  vec4(col,1.0);
}    

float	de_0(vec3 p) // Spaghettis
{
	float	mind = 1e5;
	float	a = (t*2.)*1.+( (p.y*.015+p.x*.015+p.z *.15)  + t/3.) * 4.;
	vec3	pr = p*.55;

	rotate(pr.xy, a);

	pr.xyz -= floor(pr.xyz);
	pr -= .5;

    #ifdef SQUARE
        if (max( abs(pr.x)-.125, abs(pr.y)-.125 ) == abs(pr.x) -.125)
	       base = vec3(.7, .2, .6);
	           else if ( max( abs(pr.x)-.125, abs(pr.y)-.125 ) == abs(pr.y)-.125 )
		   	    base = vec3(.4, .6, .2);
			        mind = length(max(abs(pr.xy) -.125, .0) );
    #else
	base = vec3(sin(pr.y-pr.x+3.14/3.), sin(pr.y-pr.x+6.28/3.), sin(pr.y-pr.x+9.42/3.) );
	    mind = length( (pr.xy) )- .10615;
    #endif
    mind = max(mind, -.03); // this cut the edges (too litle radius spaghettis)
    return (mind);
}

float sdCappedCylinder( vec3 p, vec2 h )
{
	vec2 d = abs(vec2(length(p.xy),p.z )) - h;
	return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

float	de_1(vec3 p) // Cylinder
{
    return (sdCappedCylinder(p, vec2(3., 20.) ) );
}

float	de_2(vec3 p)
{
    vec2 q = vec2(length(p.yx) , p.z );
    #ifdef FANCY
    	   p.xy += .125*rot(q.xy, vec2(-1.5+sin(t+p.x/40.+p.z/40.)*2., -1.5+sin(t+p.z/40.+p.y/40.)*2.));
    	   p.yz += .125*rot(p.yz, vec2(-1.5+cos(t+q.x/40.)*1., -1.5+sin(t+p.z/40.)*1.));
	   	p.yx += .125*rot(p.yx, vec2(p.z+sin(t)/10., p.z+(2.*t)/10.) );
		#endif
    p.y += sin(t)-length(q.y)/2.; // introduce a discontinuity at p.z==0. but I like it anyway
    p.y += sin(t*4.+p.z*.5)*2.3;
    p.z+=sin(t*.5)*10.;
    return (de_0(p)/1.+de_1(p)/20.);
}

float	scene(vec3 p)
{
    float	mind = 1e5;
    vec2	rot = vec2( 0.54, 0.84 );			// cos(1.), sin(1.)
    p.xz *= mat2(rot.x, rot.y, -rot.y, rot.x);			// view from angle

    mind = de_2(p);
    
    return(mind);
}


vec4	march(vec3 pos, vec3 dir)
{
    vec2	dist = vec2(0.0, 0.0);
    vec3	p = vec3(0.0, 0.0, 0.0);
    vec4	step = vec4(0.0, 0.0, 0.0, 0.0);

    for (int i = -1; i < I_MAX; ++i)
    {
	p = pos + dir * dist.y;
        dist.x = scene(p);
        dist.y += dist.x;
        if (dist.x < E )
        {
            step.y = 1.;
            break;
        }
        step.x++;
    }
    step.w = dist.y+dist.x;
    return (step);
}

// Utilities

void rotate(inout vec2 v, float angle)
{
	v = vec2(cos(angle)*v.x+sin(angle)*v.y,-sin(angle)*v.x+cos(angle)*v.y);
}

vec2	rot(vec2 p, vec2 ang)
{
	float	c = cos(ang.x);
    float	s = sin(ang.y);
    mat2	m = mat2(c, -s, s, c);
    
    return (p * m);
}


vec3 calcNormal( in vec3 pos, float e, vec3 dir)
{
    vec3 eps = vec3(e,0.0,0.0);

    return normalize(vec3(
           march(pos+eps.xyy, dir).w - march(pos-eps.xyy, dir).w,
           march(pos+eps.yxy, dir).w - march(pos-eps.yxy, dir).w,
           march(pos+eps.yyx, dir).w - march(pos-eps.yyx, dir).w ));
}

vec3	camera(vec2 uv)
{
    float		fov = 1.;
    vec3		    forw  = vec3(0.0, 0.0, -1.0);
    vec3    		    right = vec3(1.0, 0.0, 0.0);
    vec3    		    up    = vec3(0.0, 1.0, 0.0);

    return (normalize((uv.x-.85) * right + (uv.y-0.5) * up + fov * forw));
}
