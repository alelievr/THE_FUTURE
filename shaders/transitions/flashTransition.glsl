void mainImage( in vec2 fragCoord )
{
	vec2 uv = fragCoord.xy / iResolution.xy;
    
    float value = .5 + atan(sin(iGlobalTime) / .1)/3.;
    
    float shake = sin(value*10.)/100.;
    vec2 shift = vec2(shake, 0.);
	fragColor = 
        texture(iChannel0, uv) * (1. - value) -
        texture(iChannel0, uv - shift) * abs(shake) * 50. * (1. - value) +
        texture(iChannel1, uv) * value -
        texture(iChannel1, uv - shift) * abs(shake) * 50. * value;
}
