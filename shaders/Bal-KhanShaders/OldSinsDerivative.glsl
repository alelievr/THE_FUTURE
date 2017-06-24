
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    float time = .5*iGlobalTime;
    vec2 uv = fragCoord.xy / iResolution.xy;
    uv = .5-uv;
    vec3 col = vec3(0.);
    uv.xy *= mat2( cos(time+3.14*uv.x*uv.y), sin(time+3.14*uv.x*uv.y), -sin(time+3.14*uv.x*uv.y), cos(time+3.14*uv.x*uv.y) );
    //uv.xy += .125*vec2( +sin((atan(uv.x, uv.y))+sin(time/20.+sin(2.*sin(time/20.))*(length(uv.xy))*20.)) );
    col.x += sin(3.14*sin(time*3.14)+3.14*sin(10.*uv.x*3.14-10.*uv.y*3.14));
    col.y += sin(3.14*cos(time*3.14+3.14)+3.14*sin(10.*uv.x*3.14+3.14 +10.*uv.y*3.14));
    col.z = (cos(col.x-col.y)*sin(col.x-col.y));
    col.x = (cos(col.x-col.y+3.14)*sin(col.x-col.y));
    col.y = (cos(col.x-col.y)*sin(3.14+col.x-col.y));
    col = 5.*vec3( (dFdx(col.x)+dFdy(col.x)), (dFdx(col.y)+dFdy(col.y)), (dFdx(col.z)+dFdy(col.z)) );
col = vec3(col.x, col.y, col.z);
    fragColor = vec4(col,1.0);
}
