void	mainImage(vec2 fragCoord)
{
	float t = fract(iGlobalTime / 2) * 5;

	if (t > 4)
		fragColor = vec4(0.235, 0.702, 0.443, 1);
	else if (t > 3)
		fragColor = vec4(1, 1, 0, 1);
	else if (t > 2)
		fragColor = vec4(0.541, 0.169, 0.886, 1);
	else if (t > 1)
		fragColor = vec4(1.000, 0.388, 0.278, 1);
	else if (t > 0)
		fragColor = vec4(0.255, 0.412, 0.882, 1);
}
