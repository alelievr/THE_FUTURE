// by Nikos Papadopoulos, 4rknova / 2013
// Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

#define ENABLE_SCROLLING // Enable uv scrolling

const float T_TRAN = 2.;	// Transition time
const float T_INTR = 2.;	// Intermission between in/out
const float T_PADN = 2.;	// Padding time at the end of out.
const float T_TOTL = ((2. * T_TRAN) + T_INTR + T_PADN);

vec3 transition(vec3 tex0, vec3 tex1, float t)
{
    return mix(tex0, tex1, t);	
}

void mainImage(vec2 p)
{
	vec2 uv = p.xy / iResolution.xy;

#ifdef ENABLE_SCROLLING
	uv += vec2(sin(iGlobalTime * .6) * .1, cos(iGlobalTime * .1));
#endif // ENABLE_SCROLLING
	
    float t = mod(iGlobalTime, T_TOTL);

    float ts0 =       T_TRAN;
    float ts1 = ts0 + T_INTR;
    float ts2 = ts1 + T_TRAN;
    
	if      (t < ts0) t = t / ts0;                       // Transition A
    else if (t < ts1) t = 1.;                            // Intermission
    else if (t < ts2) t = 1. - ((t - ts1) / (ts2-ts1));  // Transition B
    else              t = 0.;                            // Padding
    
    vec3 tex0 = texture(iChannel0, uv).xyz;
    vec3 tex1 = texture(iChannel1, uv).xyz;    
    vec3 r = transition(tex0, tex1, t);
	fragColor = vec4(r, 1);
}
