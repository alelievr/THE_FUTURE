#define C(uv) (1.-(step(distance(uv, vec2(0.1,0.1) ), .1+.0125*abs(sin(t*.25)) )+step(.07, distance(uv-vec2(.06,.06), vec2(0.)))))

void mainImage( in vec2 f )
{
	vec2 uv = f.xy / iResolution.xy - .5;
    float    t = iGlobalTime;
    float    r = 1.;
    r = -1.;//1.*sign(sin(uv.x*.25));
    float	a = r*(1.857+t*.251);
    mat2	rot;;// = mat2(cos(a),sin(a),-sin(a),cos(a));
//    uv *= 4.;
    //uv = abs(uv);
    //uv = fract(uv*5.5)-.5;
    uv *=.2;
    uv *= mat2(cos(t), sin(t), -sin(t), cos(t));
    fragColor = vec4(0.,0.,0.,1.0);
//  fragColor.x = C(uv);
    float s = 0.;
    for (int i = 0; i < 30; i++)
    {//r*=-1.051;
        a=.008+t*.06125;//a*=r;
        rot = mat2(cos(a),sin(a),-sin(a),cos(a));
    uv *= 1.1;
    uv *= rot;
//    uv = abs(uv);
    fragColor.x = max(C(uv),fragColor.x);
        if(fragColor.x > 0.)
            break;
        s++;
	}
//    fragColor.x = 1.-fragColor.x;
/*    if (fragColor.x == 0.)
    {
        fragColor.xyz += vec3(1., .4, .2)*sin(3000.*(f.x/iResolution.x));
        fragColor.xyz += vec3(.5, .2, 1.2)*sin(3000.*(1.-f.y/iResolution.x));
    }*/
  //  else
    fragColor.x -= .125*fragColor.x*abs(sin(2800.*(uv.x)));
    fragColor.x -= .125*fragColor.x*abs(sin(2800.*(1.-uv.y)));
    	fragColor.xyz = fragColor.x*vec3(
			  abs(sin(s*2.+1.*3.14/3.)) ,
                      abs(sin(s*2.+2.*3.14/3.)) ,
                      abs(sin(s*2.+3.*3.14/3.)) 
                      );
    
}
