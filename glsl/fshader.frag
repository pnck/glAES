uniform sampler2D qt_Texture0;
uniform sampler1D qt_Texture1;
varying vec4 texCoord0;

void main(void)
{
    vec4 color = texture2D(qt_Texture0,texCoord0.st);
    float rr = texture1D(qt_Texture1,color.r).r;
    float gg = texture1D(qt_Texture1,color.g).g;
    float bb = texture1D(qt_Texture1,color.b).b;
    gl_FragColor = vec4(rr,gg,bb,1);
}
