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
float udBox( vec3 p, vec3 b );
float sdTorusAbs( vec3 p, vec2 tt );
float sdCappedCylinder( vec3 p, vec2 h );
float train(vec3	p);
float get_face(vec3 p);

uniform float	xOffset = 0;

float g; //coloring id
float t; // time
float a; // angle
vec3  ss;// tmp var recording old ray position

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
    vec2 ff = f;
    ff.x += xOffset;
    vec2 R = iResolution.xy,
          uv  = vec2(ff-R/2.) / R.y;
	  vec3	dir = camera(uv);
    vec3  pos = vec3(.5, .5, 0.0+t*4.);
    rotate(dir.zx, cos(t)*.25);
    vec2	   inter = (march(pos, dir));
    fragColor.xyz = vec3(inter.y*.051 - (inter.x)*.001);
    fragColor.xyz *= vec3(abs(sin(1.04+t) ), abs(sin(2.09+t) ), abs(sin(3.14+t) ));
    fragColor.xyz += /*(1.-sin(t+1.57))**/blackbody((15.-(2.*inter.y-.1*inter.x) )*50.);
    //fragColor.xyz += /*(1.-sin(t))**/vec3(abs(sin(t+1.04+g)), abs(sin(t+2.09+g)), abs(sin(t+3.14+g)))*inter.x*.005;
//    fragColor.xyz *= (1.1-length(uv)*1.);
      fragColor.w = 1.;
}

float	rails(vec3	p)
{
	float mind = 1e5;
    float     z_p = step(mod(p.z,1.), .5);
    p.x -=.5;
//    p.z -=t*4.5;
    p.z-= z_p*(floor(p.z));
    rotate(p.zx, 1.57* z_p );
    mind = length(p.xy)-.05;
    
    return (mind);
}
#define SEE_ALL
float	scene(vec3 p)
{
    float mind;
    float l_train = 1e5;
    float l_rails;
    float scale = 1.0;
    float r2,k;
    vec3  p_train = p;
    p_train.z-=-t*4.;
    
    a = cos( .8*(p.z *.5) +t*1.5);
    //rotate(p_train.xy, a);
    //l_train = train(p_train);
    l_rails = rails(p);
    l_train = min(l_train, l_rails);
//    p.y+=-.5;
    p.x+=.5;
    rotate(p.yx, 1.57);//(a) );
    ss = p;
    for( int i=0; i<4; i++ )
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
    mind = (-.085*(1.-sin(t))*0.+0.25*(abs(p.x))/scale);
    #else
    mind = (/*-.085*(1.-sin(t))*/+0.25*(abs(p.x)+length(fract(ss.xz)-.5 )*.25)/scale);
    #endif
    mind = min(mind, l_train);
//    mind = l_train;
    if (mind == l_train)
        ss = vec3(1e5);
    return (mind);
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
//    	rotate(dirr.zx, .05*dist.y*sin(t*1.5));
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

float	get_face(vec3 p)
{
	float	ret;
    
    ret = length(vec3(p.x,p.y,p.z*1.75)-vec3(3.,-1.2,1.5) ) - 1.;
    ret = min(ret, (length(vec3(p.x,p.y,p.z*1.25)-vec3(2.6,-.75,1.7))-.3) );
    ret = min(ret, (length(vec3(p.x,p.y,p.z*1.25)-vec3(3.3,-.75,1.7))-.3) );
    p+=-vec3(3.,-1.5,1.4);
    rotate(p.xz, 1.57);
    rotate(p.yx, .40);
    ret = min(ret, (sdTorus(p, vec2(.2,.1) ) ) );
    return (ret);
}

float	train(vec3	p)
{
    float	mind = 1e5;
    float	thomas_face = 1e5;
    p.x-=3.5;
//    p.z+=3.;
    p.y-=2.3;
//    p.y -= -3.5;
    rotate(p.zx, 3.14 );
//    rotate(p.yx, -t*1.5);
      p.z+=1.5;
    p.z += 5.;
    p.x += 2.*cos(t*3.);
    mind = min(mind, udBox(p-vec3(3.,-1.9,-1.), vec3(.25,.25,.5)*.35 ) );
    mind = min(mind, sdCappedCylinder((vec3(p.z,p.x,p.y)-vec3(-.85,3.,-1.7)), vec2(.083, .4)*.35 ) );
    mind = min(mind, length(p-vec3(3.,-1.55,-0.85) ) -.166*.35);
    mind = max(mind, - (length(p-vec3(3.,-1.5,-.85) ) -.183*.35 ) );
    mind = min(mind, sdTorusAbs((p-vec3(3.,-2.,-1.)), vec2(.083*.35) ) );
//    thomas_face = get_face(p);
    mind = min(mind, thomas_face );
    return mind;
}

float udBox( vec3 p, vec3 b )
{
  return length(max(abs(p)-b,0.0));
}

float sdTorusAbs( vec3 p, vec2 tt )
{
    p.xz = abs(p.xz)-.33*.35;
    //rotate(p.zy, t*100.);
    vec2 q = vec2( (length(p.zy)-tt.x),p.x);

    return (length(q)-tt.y);
}

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
