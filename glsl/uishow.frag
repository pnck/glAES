#version 130
uniform sampler2D rawTex;
in vec2 coord;

void main(void)
{
    gl_FragColor = texture2D(rawTex,coord);
}
