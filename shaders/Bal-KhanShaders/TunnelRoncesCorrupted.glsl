
uniform float cameraDirection = 1;
float	t;

#define	NUMBALLS	2
#define I_MAX			50
#define E					0.001

vec4	march(vec3 pos, vec3 dir, vec3 balls[NUMBALLS], inout vec3 b);
vec3	camera(vec2 uv);
vec3	calcNormal(in vec3 pos, float e, vec3 dir, vec3 balls[NUMBALLS], vec3 ba);
void	init(inout vec3 balls[NUMBALLS]);
vec3	color_func(vec3 pos, vec3 dir, vec3 balls[NUMBALLS]);

vec3 h2rgb(float c) {
	vec3 p = abs(fract(c + vec3(3,2,1)/3.) * 6. - 3.);
	return clamp(p - 1., 0., 1.);
}

void mainImage(in vec2 f)
{
    t = iGlobalTime;
    vec3	col = vec3(0., 0., 0.);
    vec2	uv  = vec2(.35+f.x/iResolution.x, f.y/iResolution.y);
    vec3	dir = camera(uv);
    vec3	pos = vec3(.0, .0, 20.0);

    vec3	BALLS[NUMBALLS];
    vec3	b = vec3(.85, .3, .3);

    pos.z -= t*5.;

    vec4  inter = (march(pos, dir, BALLS, b));

    vec3  base = h2rgb(iGlobalTime * 0.02) * 1.5;

    col.xyz = (base-log(inter.w*.17));
    fragColor =  vec4(col,1.0);
}    

#define POWER	9.
#define PI		3.14

void rotate(inout vec2 v, float angle)
{
	v = vec2(cos(angle)*v.x+sin(angle)*v.y,-sin(angle)*v.x+cos(angle)*v.y);
}

float udBox( vec3 p, vec3 b )
{
  return length(max(abs(p)-b,0.0));
}

float	de(vec3 p, inout vec3 b) // tentacules, trou au milieu
{//p.z += sin(p.x);
       float	mind = 1e5;//p.z = mod(p.z,4.); // artifictus disparatuss
       float	a = cos( cos(t/3.+(p.x*p.x+p.y*p.y)*.005)*- cos(t/4.+(p.z*p.z) *.03)) * 4.;
//       return (mind);
	 //a *= (a)+sin(+t/3.); // edging the rotation
	 vec3	pr = p*.35;
//a=1.57;
//a = 0.; // pour transition de cylindres plat a cette anim
    rotate(pr.xy, (cos(a)*cos(t*8.+length(p.z+p.x+p.y) )*.05+sin(t)/*+(p.z)*/ ));
    //pr.z += 2.1*cos(t+pr.x+pr.y);
    //pr.z += 4.*cos(a/3.);

    //rotate(pr.xy, (a/*+(p.z)*/ ));
   // if (pr.z < 2.5)
     //       return (mind);
     pr.xy*=2.;//2.
     pr.xyz -= floor(pr.xyz);///2.;
     pr -= .5;

    //pr .xy *= abs(pr.xy) ;/// 5.;
    //if ( /*length(p.z) < 19. && */!(abs(p.x) > 17.) ) // || length(pr.y) > .005)
    //mind = length(pr.xy) - (.2 );//(.05*sin(t*25.)*(sin(+p.z*5.5)) + .1); //+p.z dans le sinus pur bosseler
    //mind = length(max(abs(pr.xy) -.125, -0.) )-(.05*abs(sin(p.z*8.)) );
    //b.z *= mind+(.05*abs(sin(t*5.+p.z*4.)) + .1);
    //b.x = b.y = b.z;
    
    mind = length(-abs(pr.zz)+abs(pr.xy)) - .1;
    
    return (mind);
}

float sdCappedCylinder( vec3 p, vec2 h )
{
  vec2 d = abs(vec2(length(p.xy),p.z)) - h;
  return min(max(d.x,d.y),0.0) + length(max(d,0.0));
}

float	de_1(vec3 p)
{
	float	mind = 1e5;
	vec3	pr = p;
	float	a = cos(+ t )*(p.z *.35 +t/100.);// *4.;
	//pr *= 2.;
	//pr.z += a;
	//rotate(pr.xy, a );

	
//	pr.zy = ( pr.zy * mat2(cos(t), sin(t), -sin(t), cos(t) ) );

//	pr -= floor(pr);
//	pr -= 4.1;

	vec2  q;// = vec2(length(pr.yx) - 4., pr.z+sin(pr.y*pr.x) );
	q = vec2(length(pr.yx) - 4., pr.y );
	
	mind = length(q) - 3.5-q.y/7.;

	return mind;
    //return (mind= max( length(pr) -3.4, mind) );
}

vec2	rot(vec2 p, vec2 ang)
{
	float	c = cos(ang.x);
    float	s = sin(ang.y);
    mat2	m = mat2(c, -s, s, c);
    
    return (p * m);
}

float	de_4(vec3 p)
{
	float	mind = 1e5;
	vec3	pr = p;
//	float	a = cos(+ t )+(p.z *.35 +t/100.);// *4.;
	//pr *= 2.;
	//pr.z += a;
	//rotate(pr.xy, a );

	
//    pr.zy = ( pr.zy * mat2(cos(t), sin(t), -sin(t), cos(t) ) );

//    pr -= floor(pr);
//    pr -= 4.1;

      vec2  q;// = vec2(length(pr.yx) - 4., pr.z+sin(pr.y*pr.x) );
      q = vec2(length(pr.yx) - 4., pr.z );
    q.y = rot(q.xy, vec2(-1., 0.)).x;
    
    mind = length(q) - 3.5;//-q.y/7.;

    return mind;
    //return (mind= max(  (length(pr) -3.4), mind) );
}


float	de_3(vec3 p);
float	old_de(vec3 p);
float	de_2(vec3 p, inout vec3 b)
{
    //return (de(p, b));
    //return de_1(p);
    //return (mix(de_1(p), de(p, b), (abs(sin(t/10.)) ) ) );
    //return(old_de(p) );
    //return (de(p, b) );
//    return(de(p,b) );
    return (de(p, b)-de_4(p)/8.);
    return(mix(de(p, b), de_4(p) , .08) );
    //return (smoothstep(0., 1. , abs(sin(t+de_4(p)/de(p,b))) ) );
    //return (-log(exp(-2.*de_4(p))+exp(-2.*de(p,b) ) )/2. );
//return (mix(de_4(p), de(p, b), min(abs(sin(t/10.)) , .9) ) ); // yay
    //return sdCappedCylinder(p, vec2(2. , 4.5) );
 //   p.x += sin(p.x+p.y+p.z);
    //return (mix(sdCappedCylinder(p, vec2(2. , 4.5) ), de_3(p), .9 ) );
    //return de_3(p);
}

float	de_3(vec3 p)
{
	float	mind = 1e5;//p.z = mod(p.z+30., 60.);//sin(p.xy+t)-abs(sin(p.xy+t+1.) );
    vec3	pr = p*.35;
    float	a = t+pr.z*.75;//( sin(t*.3) );//+(+pr.z* .1) ) * 2.;
    rotate(pr.xy, (a) );//+atan(p.y, p.x) )*2. );
    
    mind = length(pr.z+pr.xy+sin(t*3.)) - ( /*(pr.z+p.x+p.y)*.05*/+ .2); //+p.z dans le sinus pur bosseler
    return (mind);
}

float	old_de(vec3 p)
{
	p.z = sin(p.z);
    p.yz = (p.yz * mat2(cos(t/7.),sin(t/7.),-sin(t/7.),cos(t/7.)) );
    float mobius;
    mobius = (10.0-1.0/(POWER)) * atan( PI*(p.y*p.x)*abs( sin(t/7.) ), PI*(-p.y*p.x)*abs( cos(t/7.) ));
    p.x = length(p.xy) - 1.2;//+cos(t/3.)*2.;
    
    rotate(p.xz, mobius);
    float m = (1.0 - ((1.0+POWER)))/(1.0*PI);
    float angle = floor(0.5 + m * (PI/2.0-atan(p.x,p.z)))/m;
    rotate(p.xz, angle);
    p.x -= 0.8+(cos(t)/2.4);//4.8;//;
    return length(p.xz)-0.1;//+abs(sin(t*2.0))*0.1;
}

float	plane_de(vec3 p)
{
	float	ret = dot(p, vec3(1.0, 1.0, -1.) ) + 1. ;
  	return ret;
}


float	scene(vec3 p, vec3 balls[NUMBALLS], inout vec3 b)
{//p.z-=20.;
    vec2	rot = vec2( cos(t/2.*cameraDirection), sin(t/2.*cameraDirection) );//p.z += sin(t*10.)*4.;
    //p.zx = ( p.zx * mat2( rot.x, rot.y, -rot.y, rot.x ) );
    //p.zx = (p.zx * mat2(cos(1.), sin(1.), -sin(1.), cos(1.)) );
    p.x += rot.x*5.;
    p.y += rot.y*5.;
    //p.x+=2.;
    //p.z += 10.;
    
    //p.xy = (p.xy * mat2(rot.x , rot.y, -rot.y, rot.x) );
    float  mind = 1e5;

    mind = de_2(p, b);
    
    return(mind);
}



vec4	march(vec3 pos, vec3 dir, vec3 balls[NUMBALLS], inout vec3 b)
{
    vec2	dist = vec2(0.0, 0.0);
    vec3	p = vec3(0.0, 0.0, 0.0);
    vec4	step = vec4(0.0, 0.0, 0.0, 0.0);

    for (int i = -1; i < I_MAX; ++i)
    {
	p = pos + dir * dist.y;
        //p.z -=20.;
        dist.x = scene(p, balls, b);
        dist.y += dist.x;
        if (dist.x < E )
        {
            step.y = 1.;
//            if (dist.y < 4.)
  //              step.y = 0.;
            break;
        }
        step.x++;
    }
    step.w = dist.y+dist.x;
    return (step);
}

vec3	color_func(vec3 pos, vec3 dir, vec3 balls[NUMBALLS])
{
    vec2	dist = vec2(0.0, 0.0);
    vec3	p = vec3(0.0, 0.0, 0.0);
    vec3	step = vec3(0.0, 0.0, 0.0);

    for (int i = -1; i < I_MAX; ++i)
    {
	p = pos + dir * dist.y;
        //dist.x = scene(p, balls);
        dist.y += dist.x;
        if (dist.x < E )
        {
            step.y = 1.;
            break;
        }
        step.x++;
    }
    step.x = (step.x/150. );
    step.y = sin(step.x+t/3.);
    step.z = sin(.78+step.x+t/2.);
    step.x = sin(1.57+step.x+t);
    return (step);
}

void	init(inout vec3 balls[NUMBALLS])
{
	for(int i = 0; i < NUMBALLS; i++)
	{
		balls[i].x = 0.;//5.*sin(t+i);
			   balls[i].y = 0.;//5.*sin(i*t-i);
			   	      balls[i].z = 0.;
				      }
}


// Utilities

vec3 calcNormal( in vec3 pos, float e, vec3 dir, vec3 balls[NUMBALLS], vec3 b)
{
    //pos.z += e*dir.z*10.;
    e /= 100.;
    vec3 eps = vec3(e,0.0,0.0);

    return normalize(vec3(
           march(pos+eps.xyy, dir, balls, b).w - march(pos-eps.xyy, dir, balls, b).w,
           march(pos+eps.yxy, dir, balls, b).w - march(pos-eps.yxy, dir, balls, b).w,
           march(pos+eps.yyx, dir, balls, b).w - march(pos-eps.yyx, dir, balls, b).w ));
}

vec3	camera(vec2 uv)
{
    float		fov = 1.;
    vec3		    forw  = vec3(0.0, 0.0, -1.0);
    vec3    		    right = vec3(1.0, 0.0, 0.0);
    vec3    		    up    = vec3(0.0, 1.0, 0.0);

    return (normalize((uv.x-.85) * right + (uv.y-0.5) * up + fov * forw));
}
/*

float	t = iGlobalTime;
vec4	noise;
vec4	strong_random_color;

#define	NUMBALLS	2
#define I_MAX			100
#define E					0.001

vec4	march(vec3 pos, vec3 dir, vec3 balls[NUMBALLS], inout vec3 b);
vec3	camera(vec2 uv);
vec3	calcNormal(in vec3 pos, float e, vec3 dir, vec3 balls[NUMBALLS], vec3 ba);
void	init(inout vec3 balls[NUMBALLS]);
vec3	color_func(vec3 pos, vec3 dir, vec3 balls[NUMBALLS]);

void mainImage(out vec4 c_out, in vec2 f)
{
    vec3	col = vec3(0., 0., 0.);
    vec2	uv  = vec2(.35+f.x/iResolution.x, f.y/iResolution.y);
    vec3	dir = camera(uv);
    vec3	pos = vec3(.0, .0, 20.0);

    vec3	BALLS[NUMBALLS];
    //pos.z += 10.*strong_random_color.x;
    vec3    b = vec3(.85, .3, .3);
    vec4    inter = (march(pos, dir, BALLS, b));
    
    vec3	base = vec3(.85, .3, .3);
    
    if (inter.y == 1.)
    {
	vec3	v = pos+inter.w*dir;
        vec3	n = calcNormal(v, E, dir, BALLS, b);
        vec3	ev = normalize(v - pos);
		vec3 ref_ev = reflect(ev, n);
        vec3	light_pos   = vec3(0.0, 0.0, 10.0);
		vec3	    light_color = vec3(.3, .4, .5);
        vec3	vl = normalize(light_pos - v);
		float			 diffuse  = max(0.0, dot(vl, n));
					 float	  specular = pow(max(0.0, dot(vl, ref_ev)), 4.);
        //col.xyz = light_color * (specular) + diffuse*base;//color_func(pos, dir, BALLS);
		  col.xyz = b * (1.-inter.x/100.);
		  	  //col.xyz = base * max( (1.-inter.w/10.)*sin(t), sin(t+1.57)*inter.w/10.);
			  	    // col.xyz = base * inter.w / 10.;
    }
    
    c_out =  vec4(col,1.0);
}    

#define POWER	9.
#define PI		3.14

void rotate(inout vec2 v, float angle)
{
	v = vec2(cos(angle)*v.x+sin(angle)*v.y,-sin(angle)*v.x+cos(angle)*v.y);
}

float	de(vec3 p, inout vec3 b) // tentacules, trou au milieu
{
	float	mind = 1e5;
	float	a = sin(t/3.+ (p.z *.05)  ) * 20.;
	//a *= (a)+sin(+t/3.); // edging the rotation
	vec3   pr = p*.35;
//a=1.57;
//a = 0.; // pour transition de cylindres plat a cette anim
    rotate(pr.xy, (a// +(p.z)d ));
    
    pr.xyz -= floor(pr.xyz);
    pr -= .5;
    
    mind = length(pr.xy) - (.05*sin(t*25.)*(sin(+p.z*5.5)) + .1); //+p.z dans le sinus pur bosseler
    //mind = length(max(abs(pr) -.125 , 0.) );
    //b.z *= mind+(.05*abs(sin(t*5.+p.z*4.)) + .1);
    //b.x = b.y = b.z;
    return (mind);
}

float	de_1(vec3 p)
{
	float	mind = 1e5;
	vec3	pr = p;
	float	a = cos(+ t )*(p.z *.35 +t/100.);// *4.;
	//pr *= 2.;
	//pr.z += a;
	//rotate(pr.xy, a );

	
//	pr.zy = ( pr.zy * mat2(cos(t), sin(t), -sin(t), cos(t) ) );

//	pr -= floor(pr);
//	pr -= 4.1;

	vec2  q;// = vec2(length(pr.yx) - 4., pr.z+sin(pr.y*pr.x) );
	q = vec2(length(pr.yx) - 4., pr.z );
	
	mind = length(q) - 3.5-q.y/7.;

	return mind;
    //return (mind= max( length(pr) -3.4, mind) );
}

float	de_2(vec3 p, inout vec3 b)
{
    //return (de(p, b));
    //return de_1(p);
    return (mix(de_1(p), de(p, b), (abs(sin(t/10.)) ) ) );
}

float	old_de(vec3 p)
{
	p.z = sin(p.z);
    p.yz = (p.yz * mat2(cos(t/7.),sin(t/7.),-sin(t/7.),cos(t/7.)) );
    float mobius;
    mobius = 1.-1./POWER;//(1.0-1.0/(POWER)) * atan2( PI*(p.y*p.x)*abs( sin(t/7.) ), PI*(-p.y*p.x)*abs( cos(t/7.) ));
    p.x = length(p.xy) - 1.2+cos(t/3.)*2.;
    
    rotate(p.xz, mobius);
    float m = (1.0 - ((1.0+POWER)))/(1.0*PI);
    float angle = floor(0.5 + m * (PI/2.0-atan(p.x,p.z)))/m;
    rotate(p.xz, angle);
    p.x -= 0.8+(cos(t)/2.4);
    return length(p.xz)-0.2+abs(sin(t*2.0))*0.1;
}

float	plane_de(vec3 p)
{
	float	ret = dot(p, vec3(1.0, 1.0, -1.) ) + 1. ;
  	return ret;
}


float	scene(vec3 p, vec3 balls[NUMBALLS], inout vec3 b)
{
    vec2	rot = vec2( cos(t*2.), sin(t*2.) );//p.z += sin(t*3.)*4.;
    //p.zx = ( p.zx * mat2( rot.x, rot.y, -rot.y, rot.x ) );
    //p.x += rot.x*5.;
    //p.y += rot.y*5.;
    p.z -= 15.;
    
    //p.xy = (p.xy * mat2(rot.x , rot.y, -rot.y, rot.x) );
    float  mind = 1e5;

    mind = de_2(p, b);
    
    return(mind);
}



vec4	march(vec3 pos, vec3 dir, vec3 balls[NUMBALLS], inout vec3 b)
{
    vec2	dist = vec2(0.0, 0.0);
    vec3	p = vec3(0.0, 0.0, 0.0);
    vec4	step = vec4(0.0, 0.0, 0.0, 0.0);

    for (int i = -1; i < I_MAX; ++i)
    {
	p = pos + dir * dist.y;
        dist.x = scene(p, balls, b);
        dist.y += dist.x;
        if (dist.x < E )
        {
            step.y = 1.;
            break;
        }
        step.x++;
    }
    step.w = dist.y+dist.x;
    return (step);
}

vec3	color_func(vec3 pos, vec3 dir, vec3 balls[NUMBALLS])
{
    vec2	dist = vec2(0.0, 0.0);
    vec3	p = vec3(0.0, 0.0, 0.0);
    vec3	step = vec3(0.0, 0.0, 0.0);

    for (int i = -1; i < I_MAX; ++i)
    {
	p = pos + dir * dist.y;
        //dist.x = scene(p, balls);
        dist.y += dist.x;
        if (dist.x < E )
        {
            step.y = 1.;
            break;
        }
        step.x++;
    }
    step.x = (step.x/150. );
    step.y = sin(step.x+t/3.);
    step.z = sin(.78+step.x+t/2.);
    step.x = sin(1.57+step.x+t);
    return (step);
}

void	init(inout vec3 balls[NUMBALLS])
{
	for(int i = 0; i < NUMBALLS; i++)
	{
		balls[i].x = 0.;//5.*sin(t+i);
			   balls[i].y = 0.;//5.*sin(i*t-i);
			   	      balls[i].z = 0.;
				      }
}


// Utilities

vec3 calcNormal( in vec3 pos, float e, vec3 dir, vec3 balls[NUMBALLS], vec3 b)
{
    vec3 eps = vec3(e,0.0,0.0);

    return normalize(vec3(
           march(pos+eps.xyy, dir, balls, b).w - march(pos-eps.xyy, dir, balls, b).w,
           march(pos+eps.yxy, dir, balls, b).w - march(pos-eps.yxy, dir, balls, b).w,
           march(pos+eps.yyx, dir, balls, b).w - march(pos-eps.yyx, dir, balls, b).w ));
}

vec3	camera(vec2 uv)
{
    float		fov = 1.;
    vec3		    forw  = vec3(0.0, 0.0, -1.0);
    vec3    		    right = vec3(1.0, 0.0, 0.0);
    vec3    		    up    = vec3(0.0, 1.0, 0.0);

    return (normalize((uv.x-.85) * right + (uv.y-0.5) * up + fov * forw));
}

*/
