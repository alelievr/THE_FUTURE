/*
* License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
* Created by bal-khan
*/

#define I_MAX	150
#define E		0.001

#define L1		vec3(0., 4.75, 1.-t*4.)
#define L2		vec3(0.,-4.75, 1.-t*4.)

#define SPIKES
#define TRIPPY
#define ANIM 0. // Speed of the animation
//#define CENTERED

void	  rotate(inout vec2 v, float angle);
vec3	  calcNormal( in vec3 pos, float e, vec3 dir);
float	  march(vec3 pos, vec3 dir);
vec3	  camera(vec2 uv);
vec3	  blackbody(float Temp);
float	  scene(vec3 p);

// --globals-- //
vec3	       tech;
vec3	       h;
float	       t;
float	       mind;
float	       tess;
float	       st;
float	       ct;
float	       hit;
float	       exp_a;
float	       bass;
float	       medium;
float	       high;
// --globals-- //

const vec3     lightCol1 = vec3(.1,.3,.7);
const vec3     lightCol2 = vec3(.5,.3,.2);

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

// sebH's volumetric light : https://www.shadertoy.com/view/XlBSRz

// ------------volumetric light----------- //

vec3 evaluateLight(in vec3 pos)
{
    float distanceToL = length(L1-pos);
    float distanceToL2 = length(L2-pos);
    return (
           lightCol2 * 1.0/(distanceToL*distanceToL)
           +lightCol1 * 1.0/(distanceToL2*distanceToL2)
           )*.5;
}

// ------------volumetric light----------- //
   
void mainImage( in vec2 f )
{
	bass = texture(iChannel0, vec2(.1, .015)).x*20.;
	medium = texture(iChannel0, vec2(.5, .015)).x*20.;
	high = texture(iChannel0, vec2(.8, .015)).x*20.;
	t = iGlobalTime;
	exp_a = exp(-t*1.+3.);
	hit = 0.;
	tech = vec3(0.);
	vec2 R = iResolution.xy,
          uv  = vec2(f-R/2.) / R.y;
    h = vec3(0.);
    
    vec3	dir = camera(uv);
    #ifndef CENTERED
    rotate(dir.zy, -.35);
    vec3	   pos = vec3(.0, 8.+sin(t)*2., 15.0-t*4. );
    #else
    vec3	pos = vec3(.0, .0250, 15.0-t*4. );
    #endif
    float	inter = (march(pos, dir));

    if (hit == 1.0)
    {
    #ifdef TRIPPY
    vec3   p = inter*dir + pos;
    tech = vec3(cos(3.*length(p-vec3(high, medium, bass)*.5)+1.04), 
              				   cos(3.*length(p-vec3(high, medium, bass)*.5)+2.08), 
              				   cos(3.*length(p-vec3(high, medium, bass)*.5)+3.14) )*.5
					           -
						           vec3(cos(20.*length(p+vec3(high, medium, bass)*.5)+1.04),
    							            cos(20.*length(p+vec3(high, medium, bass)*.5)+2.08),
        							         cos(20.*length(p+vec3(high, medium, bass)*.5)+3.14) )*.5
        ;
        if (mind == tess)
            fragColor.xyz+=vec3(cos(20.*length(p)+1.04), cos(20.*length(p)+2.08), cos(20.*length(p)+3.14) )*.75;
        else
			fragColor.xyz += tech;
    #endif
        fragColor.xyz += blackbody(length(h)*20000.+(inter*.51)*(100.-min(100.,100.*exp(-t*1.+3.)) ));
	    fragColor.xyz *= (1.-length(uv)*1.); // vignette
    }
    fragColor.xyz *= .5;
    fragColor.xyz += h*1.;
    fragColor.w = 1.;
}
#undef ANIM
#define ANIM .5
float	scene(vec3 p)
{
    float	mine;
    mind = mine = tess = 1e5;
    
    mine = min(mine, length(p-L1)-.1);
    mine = min(mine, length(p-L2)-.1);

    p.xy = abs(p.xy)-2.0+2.*sin(p.z*.125+t*.052*ANIM+bass*.025);
    p *= .5;
    p+= vec3(cos(p.z+t*1.*ANIM)*1.,sin(p.z+t*1.*ANIM)*1.,0.);
    p.xy=abs(p.xy)-.5;
    p.x += cos(p.z*3.+t*2.*ANIM)*.5;
    p.y += sin(p.z*3.+t*2.*ANIM)*.5;
    
    mine = min(mine, ( -.02+.02*exp_a+1.01002-length( ( cos(p.x)*cos(p.y ) ) ) ) );
    
    #ifdef	SPIKES
    if(t > 4.)
    {
	float	ba;
    	ba = length(sin(p.xy))*(.251+0.749*exp_a);
    	p.z-=t*.5;
    	tess = min(tess,
    	          ((ba-.075)+length(sin(vec2(p.y, mod(p.z, .125) )) ))
    		            );
        p*=.5;
    	rotate(p.xy, -p.z*(7.)*3.-t);
    	tess = min(tess, length(vec2(abs(p.y)-.1+0.*.0125*cos(p.z*80.+high*20.-t*20.*ANIM),abs(p.x) ))
    	           -.006125-0.*.006125*high-.025*clamp( (40.*sin(p.z*80.+t*30.*ANIM)), .25,.75) );
		   }
    #endif
    mind = min(mine, mind);
    mind = min(mind, tess);
    return mind;
}

float	march(vec3 pos, vec3 dir)
{
    vec2	dist = vec2(0.0);
    vec3	p = vec3(0.0);

    for (int i = 1; i < I_MAX; ++i)
    {
        hit = 0.;
    	p = pos + dir * dist.y;
        dist.x = scene(p);
        dist.y += dist.x;
        h += evaluateLight(p)*.25;
        if (dist.x < E)
        {
           hit = 1.;
           break;
        }
    }
    return (dist.y);
}

// Utilities

void rotate(inout vec2 v, float angle)
{
	v = vec2(cos(angle)*v.x+sin(angle)*v.y,-sin(angle)*v.x+cos(angle)*v.y);
}

vec3	camera(vec2 uv)
{
    float   fov = 1.;
    vec3    forw  = vec3(0.0, 0.0, -1.0);
    vec3    right = vec3(1.0, 0.0, 0.0);
    vec3    up    = vec3(0.0, 1.0, 0.0);

    return (normalize((uv.x) * right + (uv.y) * up + fov * forw));
}
