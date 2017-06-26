vec3 h2rgb(float c) {
    vec3 p = abs(fract(c + vec3(3,2,1)/3.) * 6. - 3.);
    return clamp(p - 1., 0., 1.);
}

float rand(float n){return fract(sin(n) * 43758.5453123);}

float noise(float p){
	float fl = floor(p);
  float fc = fract(p);
	return mix(rand(fl), rand(fl + 1.0), fc);
}

vec2 rand2(vec2 n) { return vec2(rand(n.x), rand(n.y)); }

vec2 noise2(vec2 n) { return vec2(noise(n.x), noise(n.y)); }

void mainImage( in vec2 fragCoord ) {
    float t = iGlobalTime/4.0;
	vec2 p = (2.0*fragCoord.xy-iResolution.xy)/iResolution.y;
	vec2 mp = vec2(5.5);
    p += vec2(.0, .0);
		
	float s = 1.0;
	for (int i=0; i < 8; i++) {
		s = max(s,abs(p.x)-sin(iGlobalTime / 8.)*7.);
		p = abs(p*2.25)-mp*0.25;
        mp*=1. + (cos(iGlobalTime / 32.) / 2.);
		p *= mat2(sin(t+mp.x),-cos(t+mp.x),cos(t+mp.y),sin(t+mp.x));
        p += noise2(p) / 1.;
	}
	
	vec3 col = h2rgb(iGlobalTime / 10.)/abs(atan(p.y,p.x))/s;
	
	fragColor = vec4(col,1.0);
}

