#pragma iChannel0 shaders/test/line.glsl

const float FXAA_SPAN_MAX = 4.0;
const float FXAA_REDUCE_MUL = 1.0/8.0;
const float FXAA_REDUCE_MIN = 1.0/128.0;

void mainImage( in vec2 fragCoord ){
    vec2 offset = 1.0/iResolution.xy;
    vec2 uv = fragCoord.xy / iResolution.xy;
    
    vec3 nw = texture(iChannel0, uv + vec2(-1.0, -1.0) * offset).rgb;
    vec3 ne = texture(iChannel0, uv + vec2( 1.0, -1.0) * offset).rgb;
    vec3 sw = texture(iChannel0, uv + vec2(-1.0,  1.0) * offset).rgb;
    vec3 se = texture(iChannel0, uv + vec2( 1.0, -1.0) * offset).rgb;
    vec3 m  = texture(iChannel0, uv).rgb;

    vec3 luma = vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(nw, luma);
    float lumaNE = dot(ne, luma);
    float lumaSW = dot(sw, luma);
    float lumaSE = dot(se, luma);
    float lumaM  = dot(m,  luma);

    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
    vec2 dir = vec2(
        -((lumaNW + lumaNE) - (lumaSW + lumaSE)),
        ((lumaNW + lumaSW) - (lumaNE + lumaSE)));

    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = min(vec2(FXAA_SPAN_MAX), max(vec2(-FXAA_SPAN_MAX), dir * rcpDirMin)) * offset;

    vec3 rgbA = 0.5 * (texture(iChannel0, uv + dir * (1.0 / 3.0 - 0.5)).xyz + texture(iChannel0, uv + dir * (2.0 / 3.0 - 0.5)).xyz);
    vec3 rgbB = rgbA * 0.5 + 0.25 * (texture(iChannel0, uv + dir * -0.5).xyz + texture(iChannel0, uv + dir * 0.5).xyz);
    float lumaB = dot(rgbB, luma);
    if (lumaB < lumaMin || lumaB > lumaMax) {
        fragColor = vec4(rgbA, 1.0);
    } else {
        fragColor = vec4(rgbB, 1.0);
    }
}
