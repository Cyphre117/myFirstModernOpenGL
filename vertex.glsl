#version 150

in vec2 position;
in vec3 vColour;

out vec3 fColour;

void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
    fColour = vColour;
}
