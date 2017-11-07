#version 130
uniform sampler2D lut_math;
uniform sampler2D roundState;
in vec2 coord;

float op_Xor4(float v1, float v2, float v3, float v4)
{
    float r = texture2D(lut_math,vec2(v1,v2)).x;
    r = texture2D(lut_math,vec2(r,v3)).x;
    r = texture2D(lut_math,vec2(r,v4)).x;
    return r;
}
float op_Mul(float va, float vb)
{
    float r = texture2D(lut_math,vec2(va,vb)).y;
    return r;
}

vec4 op_MixCol(vec4 col)
{
    vec4 p0 = texture2D(roundState,vec2(0.001,coord.t));
    vec4 p1 = texture2D(roundState,vec2(0.251,coord.t));
    vec4 p2 = texture2D(roundState,vec2(0.501,coord.t));
    vec4 p3 = texture2D(roundState,vec2(0.999,coord.t));
    vec4 r;
    //   | 2 3 1 1 |
    //   | 1 2 3 1 |
    //   | 1 1 2 3 |
    //   | 3 1 1 2 |
    r.x = op_Xor4(op_Mul(p0.x,2) , op_Mul(p1.x,3) , op_Mul(p2.x,1) , op_Mul(p3.x,1));
    r.y = op_Xor4(op_Mul(p0.y,1) , op_Mul(p1.y,2) , op_Mul(p2.y,3) , op_Mul(p3.y,1));
    r.z = op_Xor4(op_Mul(p0.z,1) , op_Mul(p1.z,1) , op_Mul(p2.z,2) , op_Mul(p3.z,3));
    r.w = op_Xor4(op_Mul(p0.w,3) , op_Mul(p1.w,1) , op_Mul(p2.w,1) , op_Mul(p3.w,2));
    return r;
}

void main(void)
{
    vec4 state_p0 = texture2D(roundState,coord);
    state_p0 = op_MixCol(state_p0);
    gl_FragColor = state_p0;
}
