#define	I_MAX	100
#define	E		0.0000001
#define PI			3.14
// change this from 0 to 2
#define	  MOBIUS    0
// change this from 0 to 4
#define	  SHAPE	    0
// abs(POWER) > 1.0 and an integer
#define POWER 9.0

mat4	lookat(vec3 e, vec3 ta, vec3 up);
void	rotate(inout vec2 v, float angle);
float	DE(vec3 p);
vec4	march(vec3 pos, vec3 dir);

void mainImage( in vec2 fragCoord)
{
	vec2	uv = fragCoord.xy / iResolution.xy;
    vec3	col = vec3(0.0);

    vec3	dir = (vec3(fragCoord.xy - iResolution.xy/2.0, -iResolution.y));
    vec3	pos = vec3(2.0*cos(2.0+iGlobalTime), -3.0, 3.0);
    mat4	look = lookat(pos, vec3(cos(2.0+iGlobalTime), 0.0, 0.0), vec3(0.0, 1.0, 0.0));
    
    dir = vec3(dot(dir, look[0].xyz), dot(dir, look[1].xyz), dot(dir, look[2].xyz));
    dir = normalize(dir);
    vec4 inter = (march(pos, dir));
    col.xyz = inter.xyz;
    fragColor = vec4(col, 1.0);
}

/*
* branching ahead, prefered this than comment and uncomment ... different combinations give different results
*/

float DE(vec3 p)
{
    p.yz *= mat2(cos(iGlobalTime),sin(iGlobalTime),-sin(iGlobalTime),cos(iGlobalTime));
    float mobius;
    if (0 == MOBIUS || 2 < MOBIUS)
       mobius = (1.0-1.0/(POWER)) * atan(PI*(p.y*p.x)*sin(iGlobalTime), PI*(-p.y*p.x)*cos(iGlobalTime));
    else if (1 == MOBIUS)
    	 mobius = (1.0-1.0/(POWER)) * atan(PI*(p.z*p.x)*sin(iGlobalTime)- PI*(p.z/p.x)*cos(iGlobalTime));
    else if (2 == MOBIUS)
    	 mobius = (1.0-1.0/(POWER)) * atan(PI*(p.y*p.x)*sin(iGlobalTime)* PI*(p.y*p.x)*cos(iGlobalTime));
	 p.x = length(p.xy) - 1.2;
	 rotate(p.xz, mobius);
	 float m = (1.0 - ((1.0+POWER)))/(1.0*PI);
	 float angle = floor(0.5 + m * (PI/2.0-atan(p.x,p.z)))/m;
	 rotate(p.xz, angle);
	 p.x -= 0.8+abs(cos(iGlobalTime)/2.4);
    if (0 == SHAPE || 4 < SHAPE)
       return length(p.xz)-0.2+abs(sin(iGlobalTime*2.0))*0.1;
    else if (1 == SHAPE)
    	 return length(p.xy)-0.4-abs(sin(iGlobalTime*3.0))*0.2; // 2-torus
    else if (2 == SHAPE)
    	 return length(p.yx)-length(p.xz)-0.72+abs(sin(iGlobalTime*3.0))*0.4; // surface
    else if (3 == SHAPE)
    	 return length(p.yx)-length(p.yz)+0.2-abs(sin(iGlobalTime*2.0))*0.2; // torus construction
    else if (4 == SHAPE)
    	 return length(p.x+p.y/2.0)-0.2+abs(sin(iGlobalTime*2.0))*0.1; // torus interior from exterior
}

vec4	march(vec3 pos, vec3 dir)
{
    vec2	dist = vec2(0.0);
    vec3	p = vec3(0.0);
    vec4	step = vec4(0.0);

    for (int i = -1; i < I_MAX; ++i)
    {
	p = pos + dir * dist.y;
        dist.x = DE(p);
        dist.y += dist.x;
        if (dist.y < E)
           break;
        step.w++;
    }
    // cheap colors
    step.x = ((abs(sin(iGlobalTime)*10.0)+10.0)/dist.y)/10.0;
    step.y = ((abs(sin(6.0/PI+iGlobalTime)*10.0)+10.0)/dist.y)/10.0;
    step.z = ((abs(sin(12.0/PI+iGlobalTime)*10.0)+10.0)/dist.y)/10.0;
    return (step);
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
