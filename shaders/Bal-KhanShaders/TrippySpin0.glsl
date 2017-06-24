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
vec3	  h;

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

#define L1 vec3(-3.,2., -5.)
const vec3 lightCol = vec3(1.,.7,.51);
vec3 evaluateLight(vec3 pos)
{
    vec3	dist = L1-pos-vec3(sin(t*8.)*3.,cos(t*8.)*3.,5. );
    dist = (dist*dist*dist*dist*dist);
    float distanceToL = pow(dist.x+dist.y*dist.z, 1./5.);
    return (
           lightCol * .250/(distanceToL*distanceToL)
           );
}

bool hit;

void mainImage(out vec4 o, in vec2 f )
{
    h = vec3(0.);
    hit = false;
    t = iGlobalTime;
    vec2 R = iResolution.xy,
          uv  = vec2(f-R/2.) / R.y;
	  vec3	dir = camera(uv);
    vec4  col = vec4(0.0);
    vec3  pos = vec3(.0, .0, 5.0);

    vec2  inter = (march(pos, dir));
//  if (hit==true)
    col.xyz = blackbody((15.-inter.y*.06125+.1*inter.x)*100.);
    //col.xyz += vec3(1.)*inter.y*.002;
//    col.xyz += h;
      o = col+inter.y*.001*vec4(.0, 1.2, 1., 1.)*2.;
}

float	scene(vec3 p)
{
    float	mind = 1e5;
    float	lit  = 1e5;
    vec2	r = vec2(cos(t*.25+sin(-t*.25+p.z*.06125) ), sin(t*.25+sin(t*.25+p.z*.06125) ));
    //p.y+=5.;
    lit = length(p-L1)-.0;
    //p.yx *= mat2(r.x, r.y, -r.y, r.x ); // rotations
    //p.yz *= mat2(cos(t), sin(t), -sin(t), cos(t) );

    mind = -sdTorus(p, vec2(15.5, 3.5) );

    #ifndef EXPERIMENTAL
    p.y = max(
            ( (sin(+t+abs(p.y)-abs(p.x) ) +(+abs(p.x)-abs(p.y) ) ) )
              ,
              ( (sin(-t+abs(p.x)-abs(p.y) ) +(-abs(p.x)+abs(p.y) ) ) ) 
             )-30.5;
//    p.y = abs(sin(t*3.+abs(p.x)-abs(p.y) )) +abs(p.z)+abs(p.y)-abs(p.x*p.y);
    #else
//    p.y = max(abs(sin(t*3.+abs(p.y)+abs(p.z) -abs(p.y+p.z)*abs(p.y-p.z) ) +abs(p.y) ), .5 );
    #endif
    p.x = max(60.0, abs(p.x)+20. );
    //p.x= abs(p.x+.0050*p.y)+.5-p.y;
    p.x= abs(p.x-.0750*p.y)+.1251*p.z+.15*p.y;
    //p.z=abs(p.z);
    //p.xy *= mat2(cos(t*2.), sin(t*2.), -sin(t*2.), cos(t*2.) );
    //p.yz *= mat2(r.x, r.y, -r.y, r.x );
    p.yz *= mat2(cos(t*.25-p.z*.061+cos(-t*.25+p.z*.061+sin(t*1.+p.z*.1) )), sin(t*.25-p.z*.061+cos(-t*.25+p.z*.061+sin(t*1.+p.z*.1) )), -sin(t*.25-p.z*.061+cos(-t*.25+p.z*.061+sin(t*1.+p.z*.1) )), cos(t*.25-p.z*.061+cos(-t*.25+p.z*.061+sin(t*1.+p.z*.1) )) );
    //p.yz *= mat2(cos(t*1.5-.1*p.y), sin(t*1.5-.1*p.y), -sin(t*1.5-.1*p.y), cos(t*1.5-.1*p.y) );
    //p.xz *= mat2(cos(t*1.+1.57 ), sin(t*1.+1.57 ), -sin(t*1.+1.57 ), cos(t*1.+1.57 ) );
    mind += sdTorus(p, vec2(5./*+p.y*/, 1.75/*+abs(-p.x+p.y)*/) );//+mind*.75;
//  mind = min(mind, lit);
    return(mind);
}

void rotate(inout vec2 v, float angle)
{
	v = vec2(cos(angle)*v.x+sin(angle)*v.y,-sin(angle)*v.x+cos(angle)*v.y);
}

vec2	march(vec3 pos, vec3 dir)
{
    vec2	dist = vec2(0.0);
    vec3	p = vec3(0.0);
    vec2	s = vec2(0.0);
    vec3	dirr;

    for (int i = -1; i < I_MAX; ++i)
    {
        dirr = dir;
        /*
        rotate(dirr.xz, sin(t*2.+dist.y*.07)*.07);
        rotate(dirr.zy, sin(t*3.+dist.y*.07)*.07);*/
        rotate(dirr.xy, (t+dist.y*.007) );
    	p = pos + dirr * dist.y;
        dist.x = scene(p);
        dist.y += dist.x;
        h += evaluateLight(p);
        if (dist.x < E)// || dist.y > 2000.)
        {
           hit = true;
           break;
        }
        s.x++;
    }
    s.y = dist.y;
    return (s);
}

/*float	my(vec2 p)
{
	p = p*p*p*p*p*p;
    return(pow(p.x+p.y, 1./8.));
}*/

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
