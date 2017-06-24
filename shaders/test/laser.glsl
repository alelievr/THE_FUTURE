uniform float LaserStart = 0;
uniform float LaserEnd = 0;

float rand(float n){return fract(sin(n) * 43758.5453123);}

float noise(float p){
	float fl = floor(p);
	float fc = fract(p);
	return mix(rand(fl), rand(fl + 1.0), fc);
}

float rand2(vec2 n) { return dot(rand(n.x), rand(n.y)); }

vec3 noise3(vec3 n) { return vec3(noise(n.x), noise(n.y), noise(n.z)); }

vec3 Strand(in vec2 fragCoord, in vec3 color, in float vscale, in float timescale, in float center, in float glow)
{
    float twopi = 6.28318530718;
    float curve = center - abs(fragCoord.y - (sin(mod(fragCoord.x / 100.0 / iResolution.x * 1000.0 + iGlobalTime * timescale, twopi)) * iResolution.y * 0.25 * vscale + iResolution.y / 2.0));
    float i = clamp(curve, 0.0, 1.0);
    i += clamp((glow + curve) / glow + rand2(fragCoord) / 50., 0.0, 1.0) * 0.4 ;
    return i * color;
}

vec3 Muzzle(in vec2 fragCoord, in float timescale)
{
    float theta = atan(iResolution.y / 2.0 - fragCoord.y, iResolution.x - fragCoord.x + 0.13 * iResolution.x);
	float len = iResolution.y * (10.0 + sin(theta * 20.0 + float(int(iGlobalTime * 20.0)) * -35.0)) / 11.0;
    float d = max(-0.6, 1.0 - (sqrt(pow(abs(iResolution.x - fragCoord.x), 2.0) + pow(abs(iResolution.y / 2.0 - ((fragCoord.y - iResolution.y / 2.0) * 4.0 + iResolution.y / 2.0)), 2.0)) / len));
    return vec3(d * (1.0 + sin(theta * 10.0 + floor(iGlobalTime * 20.0) * 10.77) * 0.5), d * (1.0 + -cos(theta * 8.0 - floor(iGlobalTime * 20.0) * 8.77) * 0.5), d * (1.0 + -sin(theta * 6.0 - floor(iGlobalTime * 20.0) * 134.77) * 0.5));
}

void mainImage( in vec2 fragCoord )
{
    float timescale = 4.0;
	vec3 c = vec3(0, 0, 0);
    float glow = 0.06 * iResolution.y;
    c += Strand(fragCoord, vec3(2.0, 2.12, 4.9), 0., 0, 0, glow);
    c += Strand(fragCoord, vec3(1.0, 1.12, 2.9), 0.3, timescale * 10, 1, glow / 4);
    c += Strand(fragCoord, vec3(1.0, 1.12, 2.9), 0.3, timescale * 8.5, 1, glow / 4);
    c += clamp(Muzzle(vec2(fragCoord.x, fragCoord.y), timescale), 0.0, 1.0) * LaserEnd;
    c += clamp(Muzzle(vec2(iResolution.x - fragCoord.x, fragCoord.y), timescale), 0.0, 1.0) * LaserStart;
	fragColor = vec4(c.r, c.g, c.b, 1.0);
}

