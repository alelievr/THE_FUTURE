#pragma viewport square
/*
* License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
* Created by bal-khan
*/

#define I_MAX	60
#define E		0.01

#define LPOS		vec3(0., 0., 0.)
#define L2			 vec3(st*4.,0.,ct*4.)

//#define CAM_SHAKE

void	  rotate(inout vec2 v, float angle);
float	  sdTorus( vec3 p, vec2 t );
float	  sdCappedCylinder( vec3 p, vec2 h );
vec2	  march(vec3 pos, vec3 dir);
vec3	  camera(vec2 uv);
vec3	  blackbody(float Temp);
float	  scene(vec3 p);

// --globals-- //
vec3	       h;
float	       g; //coloring id
float	       t; // time
float	       a; // angle
vec3	       ss;// tmp var recording old ray position
vec3	       sss;
float	       mind;
float	       mine;
float	       minf;
vec3	       super_uv;
float	       st;
float	       ct;
// --globals-- //

const vec3     lightCol = vec3(1.,.7,.51);
mat2  	       rot_t;

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
    float distanceToL = length(LPOS-pos);
    float distanceToL2 = length(L2-pos);
    return (
           lightCol * 1.0/(distanceToL*distanceToL)
           //+lightCol * 1.0/(distanceToL2*distanceToL2)
           )*.5;
}

// ------------volumetric light----------- //

void mainImage(in vec2 f )
{
    mine = 1e5;
    t = iGlobalTime*.5;
    st = sin(t);
    ct = cos(t);
    rot_t = mat2(ct, st, -st, ct );

    vec2 R = iResolution.xy,
          uv  = vec2(f-R/2.) / R.y;
    h = vec3(0.);
    
    vec3	dir = camera(uv);
    vec3	pos = vec3(.0, .0, 7.50);
    #ifdef CAM_SHAKE
    rotate(dir.xz, sin(t*40.)*.06125);
    #endif
    vec2	inter = (march(pos, dir));

    fragColor.xyz = blackbody(((h.x+h.y+h.z) )*100.);
    fragColor.xyz += vec3(abs(sin(t+1.04+g)), abs(sin(t+2.09+g)), abs(sin(t+3.14+g)))*(1.-inter.y*.067515);
    fragColor.xyz += h*.125;
    fragColor.xyz *= (1.25-length(uv)*1.); // vignette
    fragColor.w = 1.;
}

float sdCy( vec3 p, vec2 h )
{
  vec2 d = abs(vec2(length(p.xz),p.y)) - h;
  return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

float	de_0(vec3 p) // Spaghettis
{
	float	ming = 1e5;
	float	a = ( (t+p.z *.5)  ) * 4.;
	vec3	pr = p;

    pr.z *= 2.;
    rotate(pr.xy, a);
    ming = max(length( abs(pr.xy)-.51*(p.z+2.1)) -.5, .0051 );
    ming = min(ming, max(length( abs(pr.xy)+.51*(p.z-2.1) ) -.5, .00051 ) );
    ming = max(ming, sdCappedCylinder(p-vec3(0.,0.,0.), vec2(1.5, 2.4) ) );
    return (ming);
}

float	scene(vec3 p)
{
    mind = 1e5;

    mine = p.z+8.;
    mine = min(mine, -(p.z)+8.);
    mine = min(mine, (p.x)+8.);
    mine = min(mine, -(p.x)+8.);
    mine = min(mine, (p.y)+4.);
    mine = min(mine, -(p.y)+4.);
    mine = min(mine, length(p)-.1);
    vec3 pole = vec3(p.x,p.z,p.y);
    pole.xy *= mix(1., .8, abs(sin(t*8.)) );
    mine = min(mine, de_0(pole) );

    minf = sdTorus(vec3(p.z,p.y,p.x), vec2(.58,.55) )
           			      -sdCy(vec3(p.x,p.y,p.z), vec2(1.6,.85) 
                  );
    mind = max(minf,-sdCy(vec3(p.z,p.y,p.x)*.65, vec2(.75,1.5) ) );
    sss = p;
    p-= L2;
/*    vec3 tata = p;
        rotate(tata.xy, +t*6.28+6.28 );
    rotate(p.xy, +t*6.28 );
    mind =	 min(mind, max(sdTorus(vec3(tata.z,tata.y,tata.x), vec2(.521,.521) ), -sdCy(vec3(tata.z,tata.y,tata.x), vec2(.61,1.5) ))
    	 	 	   					   + abs(tata.y*1.5)*abs(tata.x*.5)
									);
    tata = p;
        rotate(tata.zx, +t*6.28+4.16 );
    rotate(p.zx, +t*6.28 );
    mind =	 min(mind, max(sdTorus(vec3(tata.x,tata.y,tata.z)*.75, vec2(.521,.521) ), -sdCy(vec3(tata.x,tata.y,tata.z)*.75, vec2(.61,1.5) ))
    	 	 	   					       + abs(tata.y*1.5*.75)*abs(tata.x*.5*.75)
										);
    tata = p;
        rotate(tata.yz, +t*6.28+2.09 );
    rotate(p.yz, +t*6.28 );
    mind =	 min(mind, max(sdTorus(vec3(tata.z,tata.x,tata.y)*.5, vec2(.521,.521) ), -sdCy(vec3(tata.z,tata.x,tata.y)*.5, vec2(.61,1.5) ))
    	 	 	   					      + abs(tata.y*1.5*.5)*abs(tata.x*.5*.5)
										);
    mind = min(mind, length(p)-.1);*/
    ss = p;
    mind = min(mine, mind);

    return mind;
}

vec2	march(vec3 pos, vec3 dir)
{
    vec2	dist = vec2(0.0);
    vec3	p = vec3(0.0);
    vec2	s = vec2(0.0);

    for (int i = 1; i < I_MAX; ++i)
    {
	p = pos + dir * dist.y;
        p.xy *= rot_t;
        p.zx *= rot_t;
        dist.x = scene(p);
        dist.y += dist.x;
        h += evaluateLight(p)*.251125;
        if (dist.x < E || dist.y > 20.)
        {
            if(dist.x == minf)
	                p=super_uv=sss;
            if(dist.x != minf && dist.x != mine)
                p=super_uv=ss;
		        g +=  (step(sin(20.*abs(p.y) ), .5) 
                  + step(sin(20.*abs(p.x) ), .5)
                  + step(sin(20.*abs(p.z) ), .5)
                 );
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

float sdCappedCylinder( vec3 p, vec2 h )
{
	vec2 d = abs(vec2(length(p.xy),p.z )) - h;
	return min(max(d.x,d.y),0.0) + length(max(d,0.0));
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
