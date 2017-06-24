#define I_MAX	150
#define E		0.001

/*
* License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
* Created by bal-khan
*/

//#define EXPERIMENTAL // too noisy on borders

float	  sdTorus( vec3 p, vec2 t );
vec2	  march(vec3 pos, vec3 dir);
vec3	  camera(vec2 uv);
vec2	  rot(vec2 p, vec2 ang);
float	  t;

// blackbody by aiekick : https://www.shadertoy.com/view/lttXDn

// -------------blackbody----------------- //

// return color from temperature 
//http://www.physics.sfasu.edu/astro/color/blackbody.html
//http://www.vendian.org/mncharity/dir3/blackbody/
//http://www.vendian.org/mncharity/dir3/blackbody/UnstableURLs/bbr_color.html

vec3 blackbody(float Temp)
{
	vec3 col = vec3(255.);
    col.x = 56100000. * pow(Temp,(-3. / 2.)) + 148.;
    col.y = 100.04 * log(Temp) - 623.6;
    if (Temp > 6500.) col.y = 35200000. * pow(Temp,(-3. / 2.)) + 184.;
    col.z = 194.18 * log(Temp) - 1448.6;
    col = clamp(col, 0., 255.)/255.;
    if (Temp < 1000.) col *= Temp/1000.;
    return col;
}

// -------------blackbody----------------- //


void mainImage( in vec2 f )
{
    t = iGlobalTime;
    vec2 R = iResolution.xy,
          uv  = vec2(f-R/2.) / R.y;
	  vec3	dir = camera(uv);
    vec4  col = vec4(0.0,0.,0.,1.);
    vec3  pos = vec3(.0, .0, 15.0);

    vec2  inter = (march(pos, dir));

    col.xyz = blackbody((1.1-length(uv)*1.1)*inter.x*300./inter.y);
    fragColor = col;
}

float	scene(vec3 p)
{
    float	mind = 1e5;
    vec2	r = vec2(cos(1.57+t), sin(1.57+t));
    p.yx *= mat2(r.x, r.y, -r.y, r.x ); // rotations
    p.xz *= mat2(r.x, r.y, -r.y, r.x );

    mind = -sdTorus(p, vec2(3.5, 3.5) );

    #ifndef EXPERIMENTAL
    p.y = max(abs(sin(t*3.+abs(p.y)+abs(p.z) ) +abs(p.y) ), .5 );
    #else
    p.y = max(abs(sin(t*3.+abs(p.y)+abs(p.z) -abs(p.y+p.z)*abs(p.y-p.z) ) +abs(p.y) ), .5 );
    #endif
    p.x = max(abs(p.x)+sin(t), 1.5 );

    mind += sdTorus(p, vec2(3.8, 2.75) );

    return(mind);
}

vec2	march(vec3 pos, vec3 dir)
{
    vec2	dist = vec2(0.0);
    vec3	p = vec3(0.0);
    vec2	s = vec2(0.0);

    for (int i = -1; i < I_MAX; ++i)
    {
	p = pos + dir * dist.y;
        dist.x = scene(p);
        dist.y += dist.x;
        if (dist.x < E || dist.y > 20.)
           break;
        s.x++;
    }
    s.y = dist.y;
    return (s);
}

float sdTorus( vec3 p, vec2 t )
{
	vec2 q = vec2(length(p.xy)-t.x,p.z);

    return length(q)-t.y;
}

vec3	camera(vec2 uv)
{
    float   fov = 1.;
    vec3    forw  = vec3(0.0, 0.0, -1.0);
    vec3    right = vec3(1.0, 0.0, 0.0);
    vec3    up    = vec3(0.0, 1.0, 0.0);

    return (normalize((uv.x) * right + (uv.y) * up + fov * forw));
}
