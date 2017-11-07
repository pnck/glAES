#version 130
in vec4 vertex;
in vec2 generalTexCoord;
out vec2 coord;

void main(void)
{
    gl_Position = vertex;
    coord = generalTexCoord;
}
