#define	I_MAX	100
#define	E		0.0000001
#define PI			3.14

mat4	lookat(vec3 e, vec3 ta, vec3 up);
void	rotate(inout vec2 v, float angle);
float	DE(vec3 p);
vec2	march(vec3 pos, vec3 dir);
float	sdCappedCylinder( vec3 p, vec2 h );

float	balls;
float	mind;
mat2	rot_t;

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

void mainImage( out vec4 fragColor, in vec2 fragCoord)
{
    rot_t = mat2(cos(iGlobalTime),sin(iGlobalTime),-sin(iGlobalTime),cos(iGlobalTime));
    vec2  uv = fragCoord.xy / iResolution.xy;
    vec2  f_s = fragCoord.xy - iResolution.xy*.5;
    f_s = iResolution.x*.1-abs(f_s);
    f_s *= rot_t;
    vec3   col = vec3(int(iResolution.y+f_s.x )&int(iResolution.y+f_s.y ) );
    
    vec3	dir = (vec3(fragCoord.xy - iResolution.xy/2.0, -iResolution.y));
    vec3	pos = vec3(20.0*cos(2.0+3.*iGlobalTime), 20.0*sin(2.0+3.*iGlobalTime), 69.0-2.0*cos(iGlobalTime));
    mat4	look = lookat(pos, vec3(cos(2.0+iGlobalTime), 0.0, 0.0), vec3(0.0, 1.0, 0.0));

    dir = (vec4(dir.xyz, 1.)*look).xyz;
    dir = normalize(dir);
    vec2 inter = (march(pos, dir));
    if (mind == balls && inter.y < 100. && inter.x != float(I_MAX) )
        col.xyz = vec3(0.,0.5,.4)*inter.y*.01;
    else if (inter.y < 100. && inter.x != float(I_MAX))
        col.xyz = vec3(1.,0.,.4)*inter.y*.01;
	fragColor = vec4(col, 1.0);
}

float DE(vec3 p)
{
    p.zx *= rot_t;
    balls = length(p-vec3(0.,0.,21.98))-4.;
    balls = min(balls, length(p-vec3(0.,0.,-21.98))-4.);
    balls = min(balls, sdCappedCylinder(p, vec2(2.,20.41)) ); // the poles and the balls
    float mobius;
    mobius = (1.0/(9.)) * atan(PI*(p.x), PI*(p.y) );
    p.x = length(p.xy) - 6.28;
    rotate(p.zx, iGlobalTime);
    rotate(p.xz, mobius);
    float m = (1.0 - ((1.0+9.)))/(6.28);
    float angle = floor(0.5 + m * (PI/2.0-atan(p.x,p.z)))/m;
    rotate(p.xz, angle);
    p.x -= 18.84;
    mind = min(length(p.xz)-3.14, balls);
    return mind;
}

vec2	march(vec3 pos, vec3 dir)
{
    vec2	dist = vec2(0.0);
    vec3	p = vec3(0.0);
    vec2	s = vec2(0.0);

    for (int i = -1; i < I_MAX; ++i)
    {
	p = pos + dir * dist.y;
        dist.x = DE(p);
        dist.y += dist.x;
        if (dist.y < E || dist.y >100.)
           break;
        s.x++;
    }
    s.y = dist.y;
    return (s);
}

mat4	lookat(vec3 e, vec3 ta, vec3 up)
{
    vec3 z = normalize(e - ta);    
    vec3 x = normalize(cross(up, z));
    vec3 y = cross(z, x);     

    mat4 orientation = mat4(
					x.x, y.x, z.x, 0.0,
								x.y, y.y, z.y, 0.0,
											x.z, y.z, z.z, 0.0,
														0.0, 0.0, 0.0, 1.0);

    mat4 translation = mat4(
					1.0, 0.0, 0.0, 0.0,
							0.0, 1.0, 0.0, 0.0, 
         						     	       	    0.0, 0.0, 1.0, 0.0,
														-e.x,-e.y,-e.z, 1.0);

    return (orientation * translation);
}

void rotate(inout vec2 v, float angle)
{
	v = vec2(cos(angle)*v.x+sin(angle)*v.y,-sin(angle)*v.x+cos(angle)*v.y);
}

float sdCappedCylinder( vec3 p, vec2 h )
{
	vec2 d = abs(vec2(length(p.xy),p.z )) - h;
	return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}
