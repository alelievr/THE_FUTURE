#define I_MAX	150
#define E_BULB	0.01
#define E_SPHI	0.01

/*
* License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
*/

float	bulb(vec3 pos);
float	de(vec3 pos);
mat4	lookat(vec3 eye, vec3 target, vec3 up);
vec2 	cmult(vec2 a, vec2 b);
vec4	march(vec3 pos, vec3 dir);

void mainImage( in vec2 fragCoord )
{
    vec4	col = vec4(0.0);
    vec3	dir = normalize(vec3(fragCoord.xy - iResolution.xy/2.0, -iResolution.y));
    vec3	pos = vec3(-2.0, 0.0, 2.0);
    mat4	look = lookat(pos, vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));

    dir = vec3(dot(dir, look[0].xyz), dot(dir, look[1].xyz), dot(dir, look[2].xyz));
    vec4 inter = (march(pos, dir));
//    if (inter.w >= float(I_MAX))
          col.xyz = inter.xyz;
//    else
        col.xyz += .5*vec3(1.0-inter.w*.51);
   	fragColor = col;
}

vec4	march(vec3 pos, vec3 dir)
{
    vec2	dist = vec2(0.0);
    vec3	p = vec3(0.0);
    vec4	step = vec4(0.0);

    for (int i = -1; i < I_MAX; ++i)
    {
	p = pos + dir * dist.y;
        dist.x = bulb(p);
        //dist.x = de(p);
        dist.y += dist.x;
        if (dist.x < E_BULB || dist.y > 3.)
           break;
        step.x += exp(-1.1*abs(dist.y + 2.50 * (-dist.y*1.+(p.x*p.z/1.0)) ));
        step.y += exp(-1.1*abs(dist.y + 2.50 * (-dist.y*1.+(p.x*p.z/2.0)) ));
        step.z += exp(-1.1*abs(dist.y + 2.50 * (-dist.y*1.+(p.x*p.z/1.5)) ));
        step.w=dist.y;
    }
    return (step);
}

float	bulb(vec3 pos) // brot3D de
{
	vec3 z = pos;
	float dr = 1.0;
	float r = 0.0;
	for (int i = 0; i < 10 ; i++)
    {
	r = length(z);
	  if (r > 15.0)
            break;
		float theta = atan(sin(z.z + iGlobalTime/5.0) * z.z/r, z.z*r);
		      float phi = atan(z.y, z.x);
		      	    dr =  pow(r, 8.0 - 1.0) * 8.0 * dr + 1.50;
			       float zr = pow(r, 8.0);
			       	     theta = theta * 8.0;
				     	   phi = phi * 8.0 + 30.0 * cos(iGlobalTime/3.0);
        //theta = cmult(vec2(theta), vec2(r,1.0/r)).y;
		z = zr*vec3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta));
		  z += pos + r/dr;
		  }
		  return 0.5*log(r)*r/dr;
}

float de(vec3 pos) // sphere de
{
	vec3 z = pos;
	float dr = 1.0;
	float r = 0.0;
	for (int i = 0; i < 10 ; i++)
    {
	r = length(z);
	  if (r > 15.0)
            break;
		float theta = atan(z.z/r, z.z*r);
		      float phi = atan(z.x - z.z, z.y - z.z);
		      	    dr =  pow(r, 5.0-1.0)*7.0*dr + 1.0;
			       float zr = pow(r, 5.0);
			       	     theta = theta*5.0;
				     	   phi = phi*5.0+5.0*cos(iGlobalTime/3.0);
					       z = zr*vec3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta));
        z.xy = cmult(vec2(z.xy),vec2(z.y/dr, z.y/dr)); // inversion try
	     z += pos;
	     }
	     return 0.4*log(r)*r/dr;
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

vec2	cmult(vec2 a, vec2 b)
{
    return (vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x));
}
