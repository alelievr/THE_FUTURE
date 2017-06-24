#define I_MAX	250
#define E		0.001

/*
* License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
*/

/*
* comment/uncomment these defines
*/
#define	ROT
#define	AO
//#define ED // somewhat different rendering
//#define BYE_BYE_FPS // transparency
//#define SIMPLEX // simplex3d
//#define NOISE_OFFSET // to blur the voronoi



float	  de(vec3 pos);
float	  de(vec3 pos, vec4 rad);
float	  sdTorus( vec3 p, vec2 t );
vec4	  march(vec3 pos, vec3 dir);
vec3	  camera(vec2 uv);
vec3	  calcNormal(in vec3 pos, float e, vec3 dir);
float	  smin(float a, float b, float k);
vec2	  rot(vec2 p, vec2 ang);
vec2	  uv;
float	  t;				//time
float	  st;						//sin(time)
float	  ct;								//cos(time)
float	  vor;										//voronoi
vec3	  noise_texture;								//blurr

// a switch used when transparency is active to optimize a bit
float	    tilt;

//3D simplex noise from nikat : https://www.shadertoy.com/view/XsX3zB

// -------------the simplex--------------- //

const float F3 =  0.3333333; // magic numbers : 1/3
const float G3 =  0.1666667; //				1/6

vec3 random3(vec3 c) {
    float j = 4096.0*sin(dot(c,vec3(17.0, 59.4, 15.0)));
    vec3 r;
    r.z = fract(512.0*j);
    j *= .125;
    r.x = fract(512.0*j);
    j *= .125;
    r.y = fract(512.0*j);
    return r-.5;
}

float simplex3d(vec3 p) {
       vec3 s = floor(p + dot(p, vec3(F3)));
        vec3 x = p - s + dot(s, vec3(G3));
	 
	  vec3 e = step(vec3(0.0), x - x.yzx);
	   vec3 i1 = e*(1.0 - e.zxy);
	    vec3 i2 = 1.0 - e.zxy*(1.0 - e);
	     
	      vec3 x1 = x - i1 + G3;
	       vec3 x2 = x - i2 + 2.0*G3;
	        vec3 x3 = x - 1.0 + 3.0*G3;
		 
		  vec4 w, d;
		   
		    w.x = dot(x, x);
		     w.y = dot(x1, x1);
		      w.z = dot(x2, x2);
		       w.w = dot(x3, x3);
		        
			 w = max(0.6 - w, 0.0);
			  
			   d.x = dot(random3(s), x);
			    d.y = dot(random3(s + i1), x1);
			     d.z = dot(random3(s + i2), x2);
			      d.w = dot(random3(s + 1.0), x3);
			       
			        w *= w;
				 w *= w;
				  d *= w;
				   
				    return dot(d, vec4(52.0));
}

// -------------the simplex--------------- //


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
    tilt = 0.;
    t = iGlobalTime /4.;
    st = sin(t);
    ct = cos(t);
    vor = 0.;

    vec2 R = iResolution.xy;
          uv  = vec2(f-R/2.) / R.y;
	  vec3	dir = camera(uv);
    vec4  col = vec4(0.0);
    vec3  pos = vec3(.0, .0, 11.0);

    vec4  inter = (march(pos, dir));

    /*
    * lighting originally taken from gltracy : https://www.shadertoy.com/view/XsB3Rm
    */    
    if (inter.w <= 20.) // else ray didn't hit anything
    {
	vec3	v = pos+inter.w*dir;
        #ifdef	BYE_BYE_FPS
        vec3	n = vec3(0.);
        if (tilt > 0.) // only calculate normal when we need it
            n = calcNormal(v, E, dir);
        #else
        vec3	n = calcNormal(v, E, dir);
        #endif
        vec3	ev = normalize(v - pos);
		vec3 ref_ev = reflect(ev, n);
        vec3	light_pos   = vec3(10.0, 10.0, 50.0);
		vec3	    light_color = vec3(.1, .4, .7)*(1.-blackbody(vor*1500.) );
        vec3	vl = normalize(light_pos - v);
		float			 diffuse  = max(0.0, dot(vl, n));
					 float	  specular = pow(max(0.0, dot(vl, ref_ev)), 4.);
        col.xyz = light_color * (specular)+ diffuse * vec3(.6, .2, .2)*(1.-blackbody(vor*1500.) );
    }
    #ifdef AO
    // basic AO
    col.xyz += (vec3(inter.x /40.))*mix(vec3(.1, .4, .7), vec3(.6, .2, .2), st );
    #endif
    #ifdef ED
    col = col/2. + col*(50.*(dFdx(col)*dFdy(col)) )/2.;
    #endif
    fragColor = col;
}

vec3 hash( vec3 p )
{
	p = vec3(	dot(p,vec3(127.1,311.7, 520.6 )),
	  dot(p,vec3(269.5,183.3, 150.9 )),
	  			  dot(p, vec3(285.1, 422.4, 211.8 ) ) );
				  return fract( (p)*18.5453); // sin(p)
}

// I took a 2d voronoi from iq, then tweaked it a bit, original : https://www.shadertoy.com/view/MslGD8

float	  voronoi3d( in vec3 x)
{
    #ifdef	SIMPLEX
    x += .5*vec3(simplex3d(x/1.5+t) );
    #endif
    vec3 n = floor( x * 2.);
    vec3 f = fract( x * 2.);
   
   vec3 m = vec3( 8., 8., 8. );
   for (float k=-1.; k<=1.; k++)
    {
	for( float j=-1.; j<=1.; j++ )
        {
			for( float i=-1.; i<=1.; i++ )
			     	   {
							vec3	g = vec3( (i), (j), (k) );
								    vec3       o = hash( n + g );
								    	       	   vec3  r = g - f + (.5+.5*sin( t*4. +6.2831*o));
										   	     float d = dot( r, r );
											     	       if( d < m.x )
												       	       	   m = vec3( d,o.xy);
														       }
        }
    }
    return float( ( (m.x) ) );
}

float	scene(vec3 p)
{
    float	mind = 1e5;
    float	ming = 1e5;
    float	minh = 1e5;

    mind = (sdTorus(p, vec2(7.5, .5)+ct*2.*exp(-t*4.+.75) ) - 1.25 )-(vor )/7.;
    ming = (sdTorus(p+vec3(0.,0., 0.+ct), vec2(4.5, .5)-ct*1.*exp(-t+2.) ) - 1. )+(vor )/7.;
    minh = (sdTorus(p, vec2(3., 0.)-ct*2.*exp(-t+.5) ) - 2. )-(vor )/20.;

    mind = min(mind, minh);
    mind = max(mind, -ming);

    #ifdef BYE_BYE_FPS
    if (vor > -.0)
    {
        mind = minh;
        tilt++;
    }
    #endif
    return(mind);
}

vec4	march(vec3 pos, vec3 dir)
{
    vec2	dist = vec2(0.0);
    vec3	p = vec3(0.0);
    vec4	s = vec4(0.0);

    for (int i = -1; i < I_MAX; ++i)
    {
	p = pos + dir * dist.y;
        
        #ifdef ROT
	       p.xz *= mat2(ct,st,-st, ct);
	       	    #endif
        
        dist.x = scene(p);
        dist.y += dist.x;
        if (dist.x < E || dist.y > 20. || tilt > 0.)
        {
            if (dist.y > 20.)
                break;
            dist.y -= dist.x;
            for (int i = -1; i < 15; ++i)
            {
                p = pos + dir * dist.y;
                #ifdef ROT
			    p.xz *= mat2(ct,st,-st, ct);
			    	        #endif

                #ifdef NOISE_OFFSET
                noise_texture = vec3(texture(iChannel0, (uv.xy) ).xy*.057, 0. );
                #endif

                vor = voronoi3d(p + noise_texture * .75 );

                dist.x = scene(p);
		            dist.y += dist.x;
                if (dist.x < E*2. || dist.y > 20.)
                {
			break;
                }
            }
            break;
        }
        s.x++;
    }
    s.w = dist.y;
    return (s);
}

// http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
float sdTorus( vec3 p, vec2 t )
{
	vec2 q = vec2(length(p.xy)-t.x,p.z);

    return length(q)-t.y;
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
    float   fov = 1.;
    vec3    forw  = vec3(0.0, 0.0, -1.0);
    vec3    right = vec3(1.0, 0.0, 0.0);
    vec3    up    = vec3(0.0, 1.0, 0.0);

    return (normalize((uv.x) * right + (uv.y) * up + fov * forw));
}
