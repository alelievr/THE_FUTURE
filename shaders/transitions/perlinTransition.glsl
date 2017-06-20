#define ANIM true
#define PI 3.14159
vec2 FragCoord;
vec4 FragColor;

// --- noise functions from https://www.shadertoy.com/view/XslGRr
// Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

mat3 m = mat3( 0.00,  0.80,  0.60,
              -0.80,  0.36, -0.48,
              -0.60, -0.48,  0.64 );

float hash( float n )
{
    return fract(sin(n)*43758.5453);
}

float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);

    f = f*f*(3.0-2.0*f);

    float n = p.x + p.y*57.0 + 113.0*p.z;

    float res = mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
                        mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
                    mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                        mix( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
    return res;
}

float fbm( vec3 p )
{
    float f;
    f  = 0.5000*noise( p ); p = m*p*2.02;
    f += 0.2500*noise( p ); p = m*p*2.03;
    f += 0.1250*noise( p ); p = m*p*2.01;
    f += 0.0625*noise( p );
    return f;
}
// --- End of: Created by inigo quilez --------------------

float myfbm( vec3 p )
{
    float f;
    f  = 0.5000*noise( p ); p = m*p*2.02;
    f += 0.2500*noise( p ); p = m*p*2.03;
    f += 0.1250*noise( p ); p = m*p*2.01;
    f += 0.0625*noise( p ); p = m*p*2.04;
    f += 0.0625/2.*noise( p ); p = m*p*2.03;
    f += 0.0625/4.*noise( p );
    return f;
}

vec3 noise3( vec3 p )
{
	if (ANIM) p += iGlobalTime;
    float fx = noise(p);
    float fy = noise(p+vec3(1345.67,0,45.67));
    float fz = noise(p+vec3(0,4567.8,-123.4));
    return vec3(fx,fy,fz);
}
vec3 fbm3( vec3 p )
{
	if (ANIM) p += iGlobalTime;
    float fx = fbm(p);
    float fy = fbm(p+vec3(1345.67,0,45.67));
    float fz = fbm(p+vec3(0,4567.8,-123.4));
return vec3(fx,fy,fz);
}
vec3 perturb3(vec3 p, float scaleX, float scaleI)
{
    scaleX *= 2.;
	return scaleI*scaleX*fbm3(p/scaleX); // usually, to be added to p
}

// --- sliders and mouse widgets

bool affMouse() 
{
	float R=5.;
	vec2 pix = FragCoord.xy/iResolution.y;
	float pt = max(1e-2,1./iResolution.y); R*=pt;

float s = 0;
	// current mouse pos
    float k = dot(pix, pix)/(R*R*.4*.4);
		if (k<1.) 
	    { if (k>.8*.8) FragColor = vec4(0.);
		     else      FragColor = vec4(s,.4,0.,1.); 
		  return true;
		}
	
	// prev mouse pos 
    k = dot(-pix,-pix)/(R*R*.4*.4);
		if (k<1.) 
	    { if (k>.8*.8) FragColor = vec4(0.);
		     else      FragColor = vec4(0.,.2,s,1.); 
		  return true;
		}
	
	return false;
}
bool affSlider(vec2 p0, vec2 dp, float v)
{
	float R=5.;
	vec2 pix = FragCoord.xy/iResolution.y;
	float pt = max(1e-2,1./iResolution.y); R*=pt;
	pix -= p0;

	float dp2 = dot(dp,dp);
	float x = dot(pix,dp)/dp2; if ((x<0.)||(x>1.)) return false;
	float x2=x*x;
	float y = dot(pix,pix)/dp2-x2; if (y>R*R) return false;

	FragColor = vec4(1.,.2,0.,1.); 
	y = sqrt(y);
	if (y<pt) return true;       // rule
	vec2 p = vec2(x-v,y);
	if (dot(p,p)<R*R) return true; // button
	
	return false;
}


void mainImage( in vec2 fragCoord ) 
{
    FragCoord=fragCoord;
	// --- events

	vec2 val = vec2(0);
	vec2 uv = fragCoord / iResolution;

	// --- shaping noise

	vec3 dir = vec3(0.,0.,1.);
	vec3 p = 4.*vec3(uv,0)+iGlobalTime*dir;
	float x = 1.*myfbm(p);
	float x1=x, x2=x;
	// shape 2 ou 3 regions separated by noisy borders
#if 1
	//x = sin(8.*PI*(x-.5));
	x  = smoothstep(-val.y+val.x,1.-val.y+1.-val.x,x);
#else
	x1 = smoothstep(-val.y+val.x,1.-val.y+1.-val.x,x);
	x2 = smoothstep(-.7+val.x,1.-.7+1.-val.x,x);
	x = abs(x2-x1);
#endif
	
	// --- texture and color sources
	uv  = fragCoord.xy/iResolution.xy;
	uv = mod(uv,vec2(1.,1.));
	vec3 T0 = texture(iChannel0,uv).rgb;
    vec3 T1 = texture(iChannel1,uv).rgb;
    vec3 T2 = texture(iChannel2,uv).rgb;
	vec3 B  = vec3(0.);
	vec3 W  = vec3(1.);
	vec3 fire = vec3(.6,0.,0.);
	
	// --- compositing
    vec3 v;
	// v = vec3(x); 
	v = mix(T0,T1,x);
	//v = mix(T0,B,x1)+mix(B,T1,x2-x1)+mix(B,T2,x2);
	float k = (1.-pow(.5+.5*cos(PI*(x-.5)),3.));
	vec3 v2;
	// v2 = T2;
	v2 = fire;
	v = mix(v2,v,k);  
	
	fragColor = vec4(v,1.0);
}
