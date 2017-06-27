#pragma iChannel0 textures/organic.jpg

vec3 random3(vec3 c) {
	float j = 4096.0*sin(dot(c,vec3(17.0, 59.4, 15.0)));
	vec3 r;
	r.z = fract(512.0*j);
	j *= .125;
	r.x = fract(512.0*j);
	j *= .125;
	r.y = fract(512.0*j);
	return r-0.5;
}

/* skew constants for 3d simplex functions */
const float F3 =  0.3333333;
const float G3 =  0.1666667;

/* 3d simplex noise */
float simplex3d(vec3 p) {
	 /* 1. find current tetrahedron T and it's four vertices */
	 /* s, s+i1, s+i2, s+1.0 - absolute skewed (integer) coordinates of T vertices */
	 /* x, x1, x2, x3 - unskewed coordinates of p relative to each of T vertices*/
	 
	 /* calculate s and x */
	 vec3 s = floor(p + dot(p, vec3(F3)));
	 vec3 x = p - s + dot(s, vec3(G3));
	 
	 /* calculate i1 and i2 */
	 vec3 e = step(vec3(0.0), x - x.yzx);
	 vec3 i1 = e*(1.0 - e.zxy);
	 vec3 i2 = 1.0 - e.zxy*(1.0 - e);
	 	
	 /* x1, x2, x3 */
	 vec3 x1 = x - i1 + G3;
	 vec3 x2 = x - i2 + 2.0*G3;
	 vec3 x3 = x - 1.0 + 3.0*G3;
	 
	 /* 2. find four surflets and store them in d */
	 vec4 w, d;
	 
	 /* calculate surflet weights */
	 w.x = dot(x, x);
	 w.y = dot(x1, x1);
	 w.z = dot(x2, x2);
	 w.w = dot(x3, x3);
	 
	 /* w fades from 0.6 at the center of the surflet to 0.0 at the margin */
	 w = max(0.6 - w, 0.0);
	 
	 /* calculate surflet components */
	 d.x = dot(random3(s), x);
	 d.y = dot(random3(s + i1), x1);
	 d.z = dot(random3(s + i2), x2);
	 d.w = dot(random3(s + 1.0), x3);
	 
	 /* multiply d by w^4 */
	 w *= w;
	 w *= w;
	 d *= w;
	 
	 /* 3. return the sum of the four surflets */
	 return dot(d, vec4(52.0));
}

const mat3 rot1 = mat3(-0.37, 0.36, 0.85,-0.14,-0.93, 0.34,0.92, 0.01,0.4);
const mat3 rot2 = mat3(-0.55,-0.39, 0.74, 0.33,-0.91,-0.24,0.77, 0.12,0.63);
const mat3 rot3 = mat3(-0.71, 0.52,-0.47,-0.08,-0.72,-0.68,-0.7,-0.45,0.56);

float simplex3d_fractal(vec3 m) {
    return   0.5333333*simplex3d(m*rot1)
			+0.2666667*simplex3d(2.0*m*rot2)
			+0.1333333*simplex3d(4.0*m*rot3)
			+0.0666667*simplex3d(8.0*m);
}
vec2 hash( vec2 p ) // replace this by something better
{
	p = vec2( dot(p,vec2(127.1,311.7)),
			  dot(p,vec2(269.5,183.3)) );

	return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}

float noise( in vec2 p )
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;

	vec2 i = floor( p + (p.x+p.y)*K1 );
	
    vec2 a = p - i + (i.x+i.y)*K2;
    vec2 o = step(a.yx,a.xy);    
    vec2 b = a - o + K2;
	vec2 c = a - 1.0 + 2.0*K2;

    vec3 h = max( 0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );

	vec3 n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));

    return dot( n, vec3(70.0) );
	
}
#define I_MAX	20
#define E		0.01
vec2	uv;
float	t;


mat3	Rot3X( float a ) {
    float c = cos( a );
    float s = sin( a );
    return mat3( 1, 0, 0,
                 0, c,-s,
                 0, s, c );
}
mat3	Rot3Y( float a ) {
    float c = cos( a );
    float s = sin( a );
    return mat3( c, 0, s,
                 0, 1, 0,
                 -s, 0, c );
}

float sdTriPrism( vec3 p, vec2 h ) {
    vec3 q = abs(p);
    return max(q.z-h.y,max(q.x*0.866025+p.y*0.5,-p.y)-h.x*0.5);
}
float	sdCylinder( vec3 p, vec3 c ) {
    return length(p.xz - c.xy) - c.z;
}

float	scene( vec3 p ) {
    float	mind = 1e5;
    if (sdCylinder( p.xzy, vec3(.0, .0, .26) ) > 3.)
        return (mind);
    mind = sdCylinder( p.xzy, vec3(.0, .0, .25) );
//    mind = sdCylinder( fract(abs(p.xzy)), vec3(.2, .25, .25) );
//    mind = min(length(simplex3d_fractal(vec3(mind, .2 * iGlobalTime, p.y)) - .1), mind);
    mind = max(length(simplex3d_fractal(vec3(mind, p.z, p.y)) - .1), mind);
//    vec4 n = vec4(0., 1., sdTriPrism( p, vec2(.2, .8) ), 0.);
//    mind = min(length(noise(vec2(mind, p.z)) -.5), mind);
//    mind = min(length(noise(vec2(p.y, mind)) -.5), mind);
/*    vec3 p2 = p;
    float d = length(p2) - .25;
    d = min(length(simplex3d_fractal(vec3(d, .2 * iGlobalTime, dot( p2, n.xyz ) + n.w)) - .1), d);
    d = max(d, length(p2)-1.4);
*/
/*
	mind = min(length(noise(vec2(iGlobalTime*.25, 1.*p.y*mind)) -.5), mind);
    mind = min(length(noise(vec2(iGlobalTime*.25, 2.*p.x*mind)) -.5), mind);
    mind = min(length(noise(vec2(iGlobalTime*.25, 2.*p.z*mind)) -.5), mind);
*/
//    vec4 n = vec4(0, .0, 0.5, 0.);
//    mind = max(length(noise(vec2(mind, dot(p,n.xyz)))) -.02, mind);
/*
    mind = min(length(noise(vec2(mind, .2 * iGlobalTime))) -.1, mind);
    mind = min(length(noise(vec2(p.y*2., mind))) -.1, mind);
*/
//    mind = min(length(texture(iChannel0, uv)) -uv, mind);
//    mind = length(noise(vec2(iGlobalTime*.25, 2.*p.y)) -.5*mind);
//    mind = mod(mind, 2.);
    return( mind);//min(mind, d) );
}
vec4	march( vec3 pos, vec3 dir ) {
    vec2	dist = vec2( 0.0 );
    vec3	p = vec3( 0.0 );
    vec4	step = vec4( 0.0 );
    for ( int i = -1; i < I_MAX; ++i )
    {
    	p = pos + dir * dist.y;
        dist.x = scene( p );
        dist.y += dist.x;
        if ( dist.x < E || dist.y > 3. )
           break;
        step.x++;
    }
    step.y = dist.x;
    step.w = dist.y;
    return ( step );
}

vec3	camera(vec2 uv) {
    float   fov = 		 1.;
	vec3    forw  = vec3( 0.0, 0.0, 1.0 );
	vec3    right = vec3( 1.0, 0.0, 0.0) ;
	vec3    up    = vec3( 0.0, 1.0, 0.0) ;

    return ( normalize( ( uv.x - 1. ) * right
                        + ( uv.y - 0.5 ) * up
                        + fov * forw ) );
}

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

void mainImage( in vec2 fragCoord ) {
    uv  = fragCoord.xy / iResolution.xy;
    if (uv.x < .15 || uv.x > .85 || uv.y < .15 || uv.y > .85)
        discard;
    uv.x *= iResolution.x / iResolution.y;
    uv = uv*1.5-.75;
	t = iGlobalTime;
    float st = sin(t);
    float ct = cos(t);
//    mat3	rot = Rot3X(0.);
	vec3	dir = camera( uv );// * rot;
    vec4	col = vec4(1., 1., 1., 1.);
    vec3	pos = vec3( .0, .0, .0);
    

    pos.z += .8*t+(.5*st+.7);
    pos.x -= .5*st;
    pos.y -= .5*-ct;
    dir *= Rot3X( ct*.6 ) * Rot3Y( st*.6 );


    vec4	inter = ( march( pos, dir ) );
	if (inter.w > 3.)
        col *= 1.-.2*texture(iChannel0, dir.xy);
	col.xyz *= 1.-blackbody(inter.x*300./inter.w);
   	fragColor = col;
}
