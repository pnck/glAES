attribute vec4 vertex;
attribute vec4 generalTexCoord;
uniform mat4 matrix;
varying vec4 texCoord0;

void main(void)
{
    gl_Position = matrix * vertex;
    texCoord0 = generalTexCoord;
}
