float map(vec3 p, out vec3 color)
{
	vec3 q = fract(p) * 2 - 1;

//	color = vec3(q / 2);
	return length(q)- .1;
}

float trace(vec3 o, vec3 r, out vec3 color)
{
	float t = 0;

	for (int i = 0; i < 100; i++)
	{
		vec3 p = o + r * t;
		float d = map(p, color);
		t += d * 0.5;
	}
	return t;
}

void		mainImage(vec2 coord)
{
	vec2 uv = (coord / iResolution) * 2 - 1;
	uv.x *= iResolution.x / iResolution.y;

vec3 iForward = vec3(0, 0, 1);
	float   fov = 1.5;
	vec3    right = normalize(cross(iForward, vec3(0, 1, 0)));
	vec3    up = normalize(cross(right, iForward));
	vec3    r = normalize(uv.x * right + uv.y * up + fov * iForward);
	vec3	o = vec3(0, 0, iGlobalTime);
	vec3	col;
	float t = trace(o, r, col);
	col = vec3(t) / 50;
	//float fog = 1 / (1 + t * t * 0.1);

	fragColor = vec4(col, 1);
}
