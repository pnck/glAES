#version 130

uniform sampler2D lut_xor;
uniform sampler2D src;
uniform float processingCoordT;//only xor this group
uniform float prevCoordT;//the prev group coord
in vec2 coord;//cur fragment

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
    vec4 frag = sampler2D(src,coord);
    if(abs(processingCoordT-coord.t) < 0.000001){//yes! xor with prev group
        vec4 prev = sampler2D(src,vec2(coord.s,prevCoordT));
        frag = op_Xor(frag,prev);
    }
    gl_FragColor = frag;
}
