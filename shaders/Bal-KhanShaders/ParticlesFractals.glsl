#define particles_count 25.
#define k 0.002
#define threshold 5.

#define zmul(a, b) vec2(a.x*b.x-a.y*b.y, a.x*b.y+b.x*a.y)
#define zinv(a) vec2(a.x, -a.y) / dot(a,a)

vec2 df(vec2 p)
{
    //return p;
    return abs(p)-0.1; // 4 points
    //return vec2(max(abs(p.x)+p.y,-p.y))-0.1; // triangle
    //return vec2(length(p))-0.1; // circle
}
void mainImage( in vec2 fragCoord )
{
	vec2 g = fragCoord.xy;
	vec2 si = iResolution.xy;
    //vec2 m = iMouse.xy/si;
    float t = iGlobalTime;
    vec3 stars = vec3(0);
    
    vec2 z = (g+g-si)/min(si.x,si.y) * 3.;
    vec2 c = 2.+vec2(sin(t*.1), cos(t*.3));//vec2(1.06,2.23);
    
    float r = 0.;
    for (float i=0.;i<25.;i++)
    {
	if (r > 15.) break;
        r = dot(z/c,z/c);
	  z += zinv( (zmul(z, z) +c*sin(i*c*3.*sin(t*k*10.))));
        vec3 col = mix(vec3(0.5,0,0.5), vec3(0,1,0.29), i/15.);
        
        vec3 acc = vec3(0);
        for (float j=0.;j<particles_count;j++)
        {
            float tt = t * j/(3.14159/(particles_count*k));
            vec2 b = vec2(cos(tt), sin(tt)) * 2.;
            acc += col/r/8./dot(z-b,z-b);
        }
        stars += acc / particles_count / 0.75;
    }
 
    fragColor = vec4(stars * 0.3,1.);
}
