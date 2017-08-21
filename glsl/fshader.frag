#version 130
uniform sampler2D rawTex;
uniform sampler1D lut_sbox;
uniform sampler1D lut_invsbox;
uniform sampler2D lut_xor;
uniform sampler1D roundState;
uniform sampler1D roundKey;
in vec2 coord;


vec4 op_Xor(vec4 Va,vec4 Vb)
{
    float rr = texture2D(lut_xor,vec2(Va.r,Vb.r)).r;
    float gg = texture2D(lut_xor,vec2(Va.g,Vb.g)).r;
    float bb = texture2D(lut_xor,vec2(Va.b,Vb.b)).r;
    float aa = texture2D(lut_xor,vec2(Va.a,Vb.a)).r;
    return vec4(rr,gg,bb,aa);
}
vec4 op_Sbox(vec4 v)
{
    float rr = texture1D(lut_sbox,v.r).r;
    float gg = texture1D(lut_sbox,v.g).r;
    float bb = texture1D(lut_sbox,v.b).r;
    float aa = texture1D(lut_sbox,v.a).r;
    return vec4(rr,gg,bb,aa);
}
vec4 op_Shift()
{
    //r0 r1 r2 r3
    //g0 g1 g2 g3
    //b0 b1 b2 b3
    //a0 a1 a2 a3
    //
    //r0 r1 r2 r3
    //g1 g2 g3 g0
    //b2 b3 b0 b1
    //a3 a0 a1 a2
/*
    float rr = texture2D(rawTex,coord.st).r;
    float gg = texture2D(rawTex,vec2(coord.s+0.25,coord.t)).g;
    float bb = texture2D(rawTex,vec2(coord.s+0.50,coord.t)).b;
    float aa = texture2D(rawTex,vec2(coord.s+0.75,coord.t)).a;
*/
    float rr = texture2D(rawTex,vec2(coord.s+0.25,coord.t)).r;
    float gg = texture2D(rawTex,vec2(coord.s+0.25,coord.t)).g;
    float bb = texture2D(rawTex,vec2(coord.s+0.25,coord.t)).b;
    float aa = texture2D(rawTex,vec2(coord.s+0.25,coord.t)).a;

    return vec4(rr,gg,bb,aa);
}

void main(void)
{
    ////  in[0],in[4],in[ 8],in[12]
    ////  in[1],in[5],in[ 9],in[13]
    ////  in[2],in[6],in[10],in[14]
    ////  in[3],in[7],in[11],in[15]
    vec4 element = texture1D(roundState,coord.s);
    vec4 key = texture1D(roundKey,coord.s);
    vec4 state = op_Xor(element,key);
    state = op_Sbox(state);

    vec4 color = texture2D(rawTex,coord.st);
/*
    rr = texture1D(qt_Texture2,rr).r;
    gg = texture1D(qt_Texture2,gg).g;
    bb = texture1D(qt_Texture2,bb).b;
*/
    vec4 sbColor = op_Sbox(color);
    gl_FragColor = vec4(sbColor.rgb,1);
    //color = op_Shift();
    //gl_FragColor = color;
}
