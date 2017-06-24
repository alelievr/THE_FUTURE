float hex(vec2 p) {
      float dx = abs(p.x*2.);
      float dy = abs(p.y*2.);
      return max(dx+dy*.57, max(dx, dy*1.15));
}

#define H0(uv) (step(hex(uv), 5.) )
//*(1.-.5*hex(uv)))
#define H1(uv) (step(hex(uv), 4.) )
#define H2(uv) (step(hex(uv), 3.) )
#define H3(uv) (step(hex(uv), 2.) )
#define H4(uv) (step(hex(uv), 1.) )
void mainImage( in vec2 f )
{
    float	t = iGlobalTime;
    t = mod(t, 9.75);
    vec2 R = iResolution.xy,
          uv  = vec2(f-R/2.) / R.y;
    vec2 U;
//  o = vec4(uv,0.5+0.5*sin(iGlobalTime),1.0);
    
    //fragColor.xyz = vec3(step(hex(uv), .1)*(1.-2.*hex(uv)) );
    
    //#define H(uv) (step(hex(uv), .1)*(1.-2.*hex(uv)))
    
    //fragColor.x = H(uv);
    
    uv.x += step(0., t-1.65)*step(t, 3.75)*(t*.1-1.66*.1);
    if (t > 3.75)
    uv.x += ((3.75-1.65)*.1);

    uv.y += step(0., t-3.75)*step(t, 5.75)*step(mod((uv.x+.5)*5., 1.), .5) * (-(t*.1-3.75*.1));
    if (t > 5.75)
        uv.y -= (5.75-3.75)*.1;
    uv.x += step(0., t-5.75)*step(t, 9.75)*step(mod((uv.y+.5)*5., 1.), .5) * (-(t*.1-5.75*.1));

//  uv.x += step(0., t-5.75)*(5.75*.1);
    
    U = (uv-.50)*5.;

    uv = fract((uv-.5)*5.)-.5;
    vec2 to = +floor(U*90.)/9.;//*200.;
    U = uv.xy;
    U.yx *= mat2(
		+sin(t*step(t, 1.66)),
        		       +cos(t*step(t, 1.66)),
        		       		      -cos(t*step(t, 1.66)),
        				      		     +sin(t*step(t, 1.66))
    );

    U.yx *= mat2(
		-sin(t*step(t, 1.66)),
        		       -cos(t*step(t, 1.66)),
        		       		      +cos(t*step(t, 1.66)),
        				      		     -sin(t*step(t, 1.66))
    );
    
    uv.yx *= mat2(
		+sin(t*step(t, 1.66)),
        		       +cos(t*step(t, 1.66)),
        		       		      -cos(t*step(t, 1.66)),
        				      		     +sin(t*step(t, 1.66))
    );
//    uv = abs(uv)-.5;
    uv.yx *= mat2(
		-sin(t*step(t, 1.66)),
        		       -cos(t*step(t, 1.66)),
        		       		      +cos(t*step(t, 1.66)),
        				      		     -sin(t*step(t, 1.66))
    );
    U+=to;
        vec3	col = vec3(
        abs(sin(+ceil(fract(U.y) )*.25*ceil(U.x)*.25+0.00)),
        abs(sin(+ceil(fract(U.y) )*.25*ceil(U.x)*.25+1.04)),
        abs(sin(+ceil(fract(U.y) )*.25*ceil(U.x)*.25+2.08))
							    );
    //uv = abs(uv)-.50;
    fragColor.x = H0(uv*5.);
    fragColor.x -= H1(uv*5.);
    fragColor.x += H2(uv*5.);
    fragColor.x -= H3(uv*5.);
    fragColor.x += H4(uv*5.);
    fragColor.xyz = fragColor.x * col;
}
