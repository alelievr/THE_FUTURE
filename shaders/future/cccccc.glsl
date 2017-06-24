float rand(float n){return fract(sin(n) * 43758.5453123);}

float noise(float p){
	float fl = floor(p);
  	float fc = fract(p);
	return mix(rand(fl), rand(fl + 1.0), fc);
}

vec3 rand3(vec3 n) { return vec3(rand(n.x), rand(n.y), rand(n.z)); }

vec3 noise3(vec3 n) { return vec3(noise(n.x), noise(n.y), noise(n.z)); }

float map(vec3 p, out vec3 color)
{
	vec3 q = fract(p) * 2. - 1.;

	color = vec3(cos(q) - vec3(.1, .1, .1));
    vec3 v = rand3(q) / 10.;
	return length(q + v);
}

float trace(vec3 o, vec3 r, out vec3 color)
{
	float t = 0.;

	for (int i = 0; i < 30; i++)
	{
		vec3 p = o + r * t;
		float d = map(p, color);
		t += d * 0.3;
	}
	return t;
}

void		mainImage(vec2 coord)
{
	vec2 uv = (coord / iResolution.xy) * 2. - 1.;
	uv.x *= iResolution.x / iResolution.y;

    vec3	iForward = normalize(vec3(sin(iGlobalTime / 10.) / 3. + 0.8, cos(iGlobalTime / 20.) / 3., .5));
	float   fov = 1.5;
	vec3    right = normalize(cross(iForward, vec3(0, 1, 0)));
	vec3    up = normalize(cross(right, iForward));
	vec3    r = normalize(uv.x * right + uv.y * up + fov * iForward);
	vec3	o = vec3(0, 0, iGlobalTime);
	vec3	col;
	float t = trace(o, r, col);
	//float fog = 1 / (1 + t * t * 0.1);

	fragColor = vec4(col, 1);
}
