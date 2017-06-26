//pragma viewport list: normal cinema, portrait, square and perfplz
#pragma viewport portrait
/*
* License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
* Created by bal-khan
*/

#define I_MAX	100
#define E		0.001

//#define SEE_ALL
//#define SPIRAL

void	  rotate(inout vec2 v, float angle);
float	  sdTorus( vec3 p, vec2 t );
vec2	  march(vec3 pos, vec3 dir);
vec3	  camera(vec2 uv);
vec3	  blackbody(float Temp);
float	  scene(vec3 p);

float	  g; //coloring id
float	  t; // time
float	  a; // angle
vec3	  ss;// tmp var recording old ray position

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
    t = iGlobalTime*.5;
    vec2 R = iResolution.xy,
          uv  = vec2(f-R/2.) / R.y;
	  vec3	dir = camera(uv);
    vec3  pos = vec3(.5, .5, 10.0-t*4.5);
    rotate(dir.zx, cos(t)*.25);
    vec2	   inter = (march(pos, dir));
    fragColor.xyz = vec3(inter.y*.051 - (inter.x)*.001);
    fragColor.xyz *= vec3(abs(sin(1.04+t) ), abs(sin(2.09+t) ), abs(sin(3.14+t) ));
    fragColor.xyz += (1.-sin(t+1.57))*blackbody((15.-(2.*inter.y-.1*inter.x) )*50.);
    fragColor.xyz += (1.-sin(t))*vec3(abs(sin(t+1.04+g)), abs(sin(t+2.09+g)), abs(sin(t+3.14+g)))*inter.x*.005;
    fragColor.xyz *= (1.1-length(uv)*1.);
    fragColor.w = 1.;
}

float	scene(vec3 p)
{
	float scale = 1.0;
    float r2,k;
    a = cos( .8*(p.z *.5) +t*1.5);
    p.y+=-.5;
    p.x+=.5;
    rotate(p.yx, (a) );
    ss = p;
    for( int i=0; i<5; i++ )
    {
        p.xyz = 1.0 - 2.0*fract(0.5*p.xyz+0.5);
        p*=1.12;
		r2 = sdTorus(p, vec2(.21,.12) );
        if(r2 > 5./float(i) )
            break;
		k = 1./(r2);
		  p     *= k;
		  	scale *= k;
			}
			ss=p*(fract(ss)+.5);
    #ifndef		SEE_ALL
    return (-.085*(1.-sin(t))+0.25*(abs(p.x))/scale);
    #else
    return (-.085*(1.-sin(t))+0.25*(abs(p.x)+length(fract(ss.xz)-.5 )*.25)/scale);
    #endif
}

vec2	march(vec3 pos, vec3 dir)
{
    vec2	dist = vec2(0.0);
    vec3	p = vec3(0.0);
    vec2	s = vec2(0.0);

    vec3	dirr;
    for (int i = 0; i < I_MAX; ++i)
    {
        dirr = dir;
    	rotate(dirr.zx, .05*dist.y*sin(t*1.5));
        #ifdef		SPIRAL
        rotate(dirr.yx, .8*dist.y*sin(t*.5));
        #endif
    	p = pos + dirr * dist.y;
        dist.x = scene(p);
        dist.y += dist.x;
        if (dist.x < E || dist.y > 20.)
        {
            p=ss;
            g = p.y;
	            g += (step(sin(5.*p.x), .5) 
             + step(sin(20.*p.x), .5) );
           break;
        }
        s.x++;
    }
    s.y = dist.y;
    return (s);
}

// Utilities

float sdTorus( vec3 p, vec2 t )
{
	vec2 q = vec2(length(p.zy)-t.x,p.x);

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

void rotate(inout vec2 v, float angle)
{
	v = vec2(cos(angle)*v.x+sin(angle)*v.y,-sin(angle)*v.x+cos(angle)*v.y);
}
