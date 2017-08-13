uniform sampler2D qt_Texture0;
uniform sampler2D qt_Texture1;
varying vec4 texCoord0;

void main(void)
{
    float lut_v = texture2D(qt_Texture1, texCoord0.st).a;
    vec4 color = texture2D(qt_Texture1,vec2(lut_v,texCoord0.t));
    gl_FragColor = vec4(color.rgb,1);
}
