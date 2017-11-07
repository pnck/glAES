#version 130

uniform sampler2D lut_xor;
uniform sampler2D roundState;
uniform sampler2D roundKey;
uniform vec2 roundCoord;
in vec2 coord;

vec4 op_Xor(vec4 Va,vec4 Vb)
{
    float rr = texture2D(lut_xor,vec2(Va.r,Vb.r)).r;
    float gg = texture2D(lut_xor,vec2(Va.g,Vb.g)).r;
    float bb = texture2D(lut_xor,vec2(Va.b,Vb.b)).r;
    float aa = texture2D(lut_xor,vec2(Va.a,Vb.a)).r;
    return vec4(rr,gg,bb,aa);
}


void main(void)
{
    vec4 state_p0 = texture2D(roundState,coord);
    vec4 key_p0 = texture2D(roundKey,roundCoord);
    state_p0 = op_Xor(state_p0,key_p0);
    gl_FragColor = state_p0;
}
