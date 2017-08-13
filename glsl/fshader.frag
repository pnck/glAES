uniform sampler2D qt_Texture0;
uniform sampler2D qt_Texture1;
varying vec4 texCoord0;

void main(void)
{
    float lut_v = texture2D(qt_Texture1, texCoord0.st).a;
    gl_FragColor = texture2D(qt_Texture0,vec2(lut_v,texCoord0.t));
}
