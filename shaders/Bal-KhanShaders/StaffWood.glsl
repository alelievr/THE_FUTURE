/*
* License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
* Created by bal-khan
*/

#define I_MAX	100
#define E		0.001

#define LPOS		vec3(0., 0., 0.)
#define L2			 vec3(+cos(t*1.)*3.,2.0,-0.+sin(t*1.)*3.)

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
float	  st;
float	  ct;
vec3	  h;
const vec3	lightCol = vec3(1.,.7,.51);

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

void mainImage(out vec4 o, in vec2 f )
{
    h = vec3(0.);
    t = iGlobalTime*.5;
    ct = cos(t);
    st = sin(t);
    vec2 R = iResolution.xy,
          uv  = vec2(f-R/2.) / R.y;
	  vec3	dir = camera(uv);
    vec3  pos = vec3(.0, 0., 7.0);
    //rotate(dir.zx, cos(t)*.25);
    vec2	     inter = (march(pos, dir));
    //o.xyz = 1.-vec3( (inter.y)*.00251);
    o.xyz += h*1.;
    //o.xyz *= vec3(abs(sin(1.04+t) ), abs(sin(2.09+t) ), abs(sin(3.14+t) ));
    o.xyz += .25*blackbody(( (inter.y) )*150.);
    //o.xyz += (1.-sin(t))*vec3(abs(sin(t+1.04+g)), abs(sin(t+2.09+g)), abs(sin(t+3.14+g)))*inter.x*.005;
    //o.xyz *= (1.1-length(uv)*1.);
}

float sdCappedCylinder( vec3 p, vec2 h )
{
	vec2 d = abs(vec2(length(p.xy),p.z )) - h;
	return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

vec3 evaluateLight(in vec3 pos)
{
    float distanceToL = length(LPOS-pos);
    float distanceToL2 = length(L2-pos);
    return (
           //lightCol * 1.0/(distanceToL*distanceToL)
           +lightCol * 1.0/(distanceToL2*distanceToL2)
           )*.5;
}

float spiral(vec2 m) {
      float r = length(m);
      float a = atan(m.y, m.x);
      float v = sin(100.*(sqrt(r)-0.02*a-.3*t*1.));
      return clamp(v,0.,1.);

}

float	de_0(vec3 p) // Spaghettis
{
	float	ming = 1e5;
	float	a = ( (t+p.z *.515)  ) * 1.;
	vec3	pr = p;



//    rotate(pr.yx, p.z*1.+t);
//pr+=.5;
//    pr.x+= .1*spiral(.05*pr.zx) ;
      //rotate(pr.yx, a);
//    pr.y+= .1*spiral(.2*p.yz) ;
    //pr.xy-=.5;
    //pr = fract(pr)-.5;
    rotate(pr.yz, t);
//    ming = max(length( abs(pr.xy)-.51*(p.z+2.1)) -.5, .0 );
//    ming = min(ming, max(length( abs(pr.xy)+.51*(p.z-2.1) ) -.5, .0 ) );
    ming = length( (pr.zy)-0. )-1.5+.051*spiral(.05*( vec2(pr.x-0.,pr.y)) )-(.5*length(pr)-2.1);//-(.5*length(p.xyz)+sign(p.z)*-.1);
    //ming = length( (pr.xy)-0. )-.0-.1*spiral(.05*( vec2(pr.z-2.,pr.x)) )-(.51*length(p.zxy-.75)-.1);
//    ming = max(-ming, sdCappedCylinder(p-vec3(0.,0.,2.), vec2(.55, 4.) ) );
    //ming = max(ming, sdCappedCylinder(p-vec3(0.,0.,2.), vec2(.55, 2.) ) );
    return (ming);
}

float	scene(vec3 p)
{
    float	mind = length(p-L2)-.1;
    p.y += 2.;
    //rotate(p.yz, (1.57) );
    //p.z+=2.;
    //rotate(p.yz, (p.z*.25) );
    mind = min(mind, de_0(vec3(p.x,p.z,p.y) ));
    
    return mind;

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
    	rotate(dirr.zx, .05*dist.y);//*sin(t*1.5));
        #ifdef		SPIRAL
        rotate(dirr.yx, .8*dist.y*sin(t*.5));
        #endif
    	p = pos + dirr * dist.y;
        //rotate(p.xz, .15*dist.y);
        dist.x = scene(p);
        dist.y += dist.x;
        h+= evaluateLight(p);
        if (dist.x < E )//|| dist.y > 20.)
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
