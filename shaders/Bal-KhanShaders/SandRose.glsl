// raymarching defines

#define I_MAX  150
#define EPSIL  0.00001

/*
* License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
*/

// these defines change the shapes

#define MAGIC
#define INVERSION
#define FIRST

//change the colors

//#define COLOR_AO
//#define COLOR_SAHARA
#define COLOR_SINS

float	de(vec3 pos);
mat4	lookat(vec3 eye, vec3 target, vec3 up);
vec2 	cmult(vec2 a, vec2 b);
vec4	march(vec3 pos, vec3 dir);

void mainImage(out vec4 fragColor, in vec2 fragCoord )
{
    vec4	col = vec4(0.0);
    vec3	dir = normalize(vec3(fragCoord.xy - iResolution.xy/2.0, -iResolution.y));
    vec3	pos = vec3(-2.0, 0.0, 2.0);
    mat4	look = lookat(pos, vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));

    dir = vec3(dot(dir, look[0].xyz), dot(dir, look[1].xyz), dot(dir, look[2].xyz));
    vec4 inter = (march(pos, dir));

    #ifdef COLOR_AO
    col.xyz = vec3(1.0-inter.w/float(I_MAX));
    #elif defined COLOR_SAHARA
    col.z += (sin(      inter.w/float(I_MAX)));
    col.x += (sin(1.57+ inter.w/float(I_MAX)));
    col.y += (sin(0.78+ inter.w/float(I_MAX)));
    #else
    col.z += (1. * (0. + sin(inter.w/float(I_MAX))));
    col.y += (1. * ( + sin(sin(iGlobalTime)*.05+inter.w/float(I_MAX))));
    col.x += pow(sin(inter.x*(0.+sin(iGlobalTime+3.14+inter.w/float(I_MAX)))/5.), 20.);
    #endif
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
        dist.x = de(p);
        dist.y += dist.x;
        if (dist.x < EPSIL)
        {step.y=dist.x;break;}
        step.w++;
    }step.x = dist.y;
    return (step);
}

float	de(vec3 pos)
{pos.yz*=mat2(cos(iGlobalTime), sin(iGlobalTime),-sin(iGlobalTime),cos(iGlobalTime));
				vec3 z = pos;
				float r = 0.0;
				      r = length(z);
 #ifdef FIRST
 	float theta = asin( z.z/r );
 #else
		float theta = asin((z.x/r+z.y/r)*z.z/r);//2nd
 #endif
	float phi = atan((z.y/r), (z.x/r));
	      theta = theta * 5.0;
	      	    phi = phi * 10.0;
 #ifdef INVERSION
        theta = cmult(vec2(theta), vec2(r,1.0/r)).x;
 #endif
 #ifdef FIRST
 	z = vec3(z.x+sin(theta), z.y-cos(theta+(iGlobalTime/2.)*20.), cos(-phi)*(z.z)*(sin(phi)/1.) );
 #else
	z = vec3(z.x+sin(z.z+theta), z.y-cos(z.z+theta+(iGlobalTime/2.)*20.), (z.z) );
 #endif
 #ifdef MAGIC
		z*=pos+length(z)/5.;
 #endif
	r = r-length(z)/4.-.2025;
	return r-.3;
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
