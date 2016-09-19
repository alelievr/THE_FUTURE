// Remnant X
// by David Hoskins.
// Thanks to boxplorer and the folks at 'Fractalforums.com'
// HD Video:- https://www.youtube.com/watch?v=BjkK9fLXXo0

// #define STEREO

vec3 sunDir  = normalize( vec3(  0.35, 0.1,  0.3 ) );
const vec3 sunColour = vec3(1.0, .95, .8);


#define SCALE 2.8
#define MINRAD2 .25
float minRad2 = clamp(MINRAD2, 1.0e-9, 1.0);
vec4 scale = vec4(SCALE, SCALE, SCALE, abs(SCALE)) / minRad2;
float absScalem1 = abs(SCALE - 1.0);
float AbsScaleRaisedTo1mIters = pow(abs(SCALE), float(1-10));
vec3 surfaceColour1 = vec3(.8, .0, 0.);
vec3 surfaceColour2 = vec3(.4, .4, 0.5);
vec3 surfaceColour3 = vec3(.5, 0.3, 0.00);
vec3 fogCol = vec3(0.4, 0.4, 0.4);
float gTime;


//----------------------------------------------------------------------------------------
float Noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
	f = f*f*(3.0-2.0*f);
	
	vec2 uv = (p.xy+vec2(37.0,17.0)*p.z) + f.xy;
	vec2 rg = vec2(0);
	return mix( rg.x, rg.y, f.z );
}

//----------------------------------------------------------------------------------------
float Map(vec3 pos) 
{
	//return (length(pos)-4.0);

	vec4 p = vec4(pos,1);
	vec4 p0 = p;  // p.w is the distance estimate

	for (int i = 0; i < 9; i++)
	{
		p.xyz = clamp(p.xyz, -1.0, 1.0) * 2.0 - p.xyz;

		// sphere folding: if (r2 < minRad2) p /= minRad2; else if (r2 < 1.0) p /= r2;
		float r2 = dot(p.xyz, p.xyz);
		p *= clamp(max(minRad2/r2, minRad2), 0.0, 1.0);

		// scale, translate
		p = p*scale + p0;
	}
	return ((length(p.xyz) - absScalem1) / p.w - AbsScaleRaisedTo1mIters);
}

//----------------------------------------------------------------------------------------
vec3 Colour(vec3 pos, float sphereR) 
{
	//scale.z = 0.0;
	vec3 p = pos;
	vec3 p0 = p;
	float trap = 1.0;
    
	for (int i = 0; i < 6; i++)
	{
        
		p.xyz = clamp(p.xyz, -1.0, 1.0) * 2.0 - p.xyz;
		float r2 = dot(p.xyz, p.xyz);
		p *= clamp(max(minRad2/r2, minRad2), 0.0, 1.0);

		p = p*scale.xyz + p0.xyz;
		trap = min(trap, r2);
	}
	// |c.x|: log final distance (fractional iteration count)
	// |c.y|: spherical orbit trap at (0,0,0)
	vec2 c = clamp(vec2( 0.3333*log(dot(p,p))-1.0, sqrt(trap) ), 0.0, 1.0);

    float t = mod(length(pos) - gTime*150., 16.0);
    surfaceColour1 = mix( surfaceColour1, vec3(.4, 3.0, 5.), pow(smoothstep(0.0, .3, t) * smoothstep(0.6, .3, t), 10.0));
	return mix(mix(surfaceColour1, surfaceColour2, c.y), surfaceColour3, c.x);
}


//----------------------------------------------------------------------------------------
vec3 GetNormal(vec3 pos, float distance)
{
	vec2 eps = vec2(distance, 0.0);
	vec3 nor = vec3(
	    Map(pos+eps.xyy) - Map(pos-eps.xyy),
	    Map(pos+eps.yxy) - Map(pos-eps.yxy),
	    Map(pos+eps.yyx) - Map(pos-eps.yyx));
	return normalize(nor);
}

//----------------------------------------------------------------------------------------
float GetSky(vec3 pos)
{
    pos *= 2.3;
	float t = Noise(pos);
    t += Noise(pos * 2.1) * .5;
    t += Noise(pos * 4.3) * .25;
    t += Noise(pos * 7.9) * .125;
	return t;
}

//----------------------------------------------------------------------------------------
float BinarySubdivision(in vec3 rO, in vec3 rD, vec2 t)
{
    for (int i = 0; i < 4; i++)
    {
     	float halfwayT = (t.y + t.x ) * .5;
		Map(rO + halfwayT*rD) < .001 ? t.x = halfwayT : t.y = halfwayT;
    }
		
//	Map(rO + halfwayT*rD) < .001 ?  t.x = halfwayT : t.y = halfwayT;
//	halfwayT = (t.y + t.x ) * .5;
//	Map(rO + halfwayT*rD) < .001 ?  t.x = halfwayT : t.y = halfwayT;
///	halfwayT = (t.y + t.x ) * .5;
//	Map(rO + halfwayT*rD) < .001 ?  t.x = halfwayT : t.y = halfwayT;
	return (t.y + t.x) * .5;;
}

//----------------------------------------------------------------------------------------
vec2 Scene(in vec3 rO, in vec3 rD, in vec2 fragCoord)
{
	float t = .05 + 0.05 * 1;
	vec3 p = vec3(0.0);
    float oldT = 0.0;
    bool hit = false;
    float glow = 0.0;
    vec2 dist;
	for( int j=0; j < 70; j++ )
	{
		if (t > 12.0) break;
        p = rO + t*rD;
       
		float h = Map(p);
        
		if( h <0.001)
		{
            dist = vec2(t, oldT);
            hit = true;
            break;
        }
        else
        {
        	glow += clamp(.05-h, 0.0, .4);
        	t +=  h + t*0.001;
	        oldT = t;
        }
 	}
    if (!hit)
        t = 1000.0;
    else
        t = BinarySubdivision(rO, rD, dist);
    return vec2(t, clamp(glow*.25, 0.0, 1.0));

}

//----------------------------------------------------------------------------------------
float Hash(vec2 p)
{
	return fract(sin(dot(p, vec2(12.9898, 78.233))) * 33758.5453)-.5;
} 

//----------------------------------------------------------------------------------------
vec3 PostEffects(vec3 rgb, vec2 xy)
{
	// Gamma first...
	rgb = pow(rgb, vec3(0.47));

	// Then...
	#define CONTRAST 1.4
	#define SATURATION 1.4
	#define BRIGHTNESS 1.2
	rgb = mix(vec3(.5), mix(vec3(dot(vec3(.2125, .7154, .0721), rgb*BRIGHTNESS)), rgb*BRIGHTNESS, SATURATION), CONTRAST);
	// Noise...
	//rgb = clamp(rgb+Hash(xy*iGlobalTime)*.1, 0.0, 1.0);
	// Vignette...
	rgb *= .5 + 0.5*pow(20.0*xy.x*xy.y*(1.0-xy.x)*(1.0-xy.y), 0.2);	

	return rgb;
}

//----------------------------------------------------------------------------------------
float Shadow( in vec3 ro, in vec3 rd)
{
	float res = 1.0;
    float t = 0.05;
	float h;
	
    for (int i = 0; i < 8; i++)
	{
		h = Map( ro + rd*t );
		res = min(6.0*h / t, res);
		t += h;
	}
    return max(res, 0.0);
}

//----------------------------------------------------------------------------------------
mat3 RotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat3(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c);
}

//----------------------------------------------------------------------------------------
vec3 LightSource(vec3 spotLight, vec3 dir, float dis)
{
    float g = 0.0;
    if (length(spotLight) < dis)
    {
		g = pow(max(dot(normalize(spotLight), dir), 0.0), 600.0) * 1.5;
    }
   
    return vec3(1.0) * g;
}

//----------------------------------------------------------------------------------------
vec3 CameraPath( float t )
{
    vec3 p = vec3(-.81 + 3. * sin(2.14*t),.05+2.5 * sin(.942*t+1.3),.05 + 3.5 * cos(3.594*t) );
	return p;
} 
    
//----------------------------------------------------------------------------------------
void mainImage( in vec2 fragCoord )
{
	float m = (iMouse.x/iResolution.x)*300.0;
	gTime = (iGlobalTime+m)*.01 + 15.00;
    vec2 xy = fragCoord.xy / iResolution.xy;
	vec2 uv = (-1.0 + 2.0 * xy) * vec2(iResolution.x/iResolution.y, 1.0);
	
	
	#ifdef STEREO
	float isRed = mod(fragCoord.x + mod(fragCoord.y, 2.0),2.0);
	#endif

	vec3 cameraPos	= CameraPath(gTime);
    vec3 camTar		= CameraPath(gTime + .01);

	float roll = 3.0*gTime;
	vec3 cw = normalize(camTar-cameraPos);

	vec3 cp = vec3(sin(roll), cos(roll),0.0);
	vec3 cu = normalize(cross(cw,cp));

	vec3 cv = normalize(cross(cu,cw));
    cw = RotationMatrix(cv, sin(-gTime*20.0)*.7) * cw;
	vec3 dir = normalize(uv.x*cu + uv.y*cv + 1.3*cw);
	mat3 camMat = mat3(cu, cv, cw);

	#ifdef STEREO
	cameraPos += .008*cu*isRed; // move camera to the right
	#endif

    vec3 spotLight = CameraPath(gTime + .03) + vec3(sin(gTime*18.4), cos(gTime*17.98), sin(gTime * 22.53))*.2;
	vec3 col = vec3(0.0);
    vec3 sky = vec3(0.03, .04, .05) * GetSky(dir);
	vec2 ret = Scene(cameraPos, dir,fragCoord);
    
    if (ret.x < 900.0)
    {
		vec3 p = cameraPos + ret.x*dir; 
		vec3 nor = GetNormal(p, ret.x*ret.x*.002+.0005);
        
       	vec3 spot = spotLight - p;
		float atten = length(spot);

        spot /= atten;
        
        float shaSpot = Shadow(p, spot);
        float shaSun = Shadow(p, sunDir);
        
       	float bri = max(dot(spot, nor), 0.0) / pow(atten, 1.5) * .15;
        float briSun = max(dot(sunDir, nor), 0.0) * .3;
        
       col = Colour(p, ret.x);
       col = (col * bri * shaSpot) + (col * briSun* shaSun);
        
       vec3 ref = reflect(dir, nor);
       col += pow(max(dot(spot,  ref), 0.0), 10.0) * 2.0 * shaSpot * bri;
       col += pow(max(dot(sunDir, ref), 0.0), 10.0) * 2.0 * shaSun  * bri;
    }
    col += vec3(pow(abs(ret.y), 2.)) * vec3(.1, .15, .25);
    col = mix(sky, col, min(exp(-ret.x+1.5), 1.0));
 
	col = PostEffects(col, xy);	
    col += LightSource(spotLight-cameraPos, dir, ret.x);
	
	#ifdef STEREO	
	col *= vec3( isRed, 1.0-isRed, 1.0-isRed );	
	#endif
	
	fragColor=vec4(col,1.0);
}

//--------------------------------------------------------------------------
