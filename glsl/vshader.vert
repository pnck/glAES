#version 130
in vec4 vertex;
in vec2 generalTexCoord;
uniform mat4 matrix;
out vec2 coord;

void main(void)
{
    gl_Position = matrix * vertex;
    coord = generalTexCoord;
}
