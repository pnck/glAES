#version 130
uniform sampler1D lut_sbox;
uniform sampler1D lut_invsbox;
uniform sampler2D roundState;
in vec2 coord;

vec4 op_Sbox(vec4 v)
{
    float rr = texture1D(lut_sbox,v.r).r;
    float gg = texture1D(lut_sbox,v.g).r;
    float bb = texture1D(lut_sbox,v.b).r;
    float aa = texture1D(lut_sbox,v.a).r;
    return vec4(rr,gg,bb,aa);
}
vec4 op_Shift(vec4 p)
{
    // p0 = r0 g0 b0 a0
    // p1 = r1 g1 b1 a1
    // p2 = r2 g2 b2 a2
    // p3 = r3 g3 b3 a3

    // p0 = r0 g0 b0 a0
    // p1 = g1 b1 a1 r1
    // p2 = b2 a2 r2 g2
    // p3 = a3 r3 g3 b3

    if(coord.s <= 0.25){
        //p0  ==>  pass
        p = p;
    }else if(coord.s <= 0.5){
        //p1
        p = p.yzwx;
    }else if(coord.s <= 0.75){
        //p2
        p = p.zwxy;
    }else {
        //p3
        p = p.wxyz;
    }
    return p;
}

void main(void)
{

    vec4 state_p0 = texture2D(roundState,coord);
    state_p0 = op_Sbox(state_p0);
    state_p0 = op_Shift(state_p0);
    gl_FragColor = state_p0;

}
