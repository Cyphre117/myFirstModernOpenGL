#version 150

uniform float brightness;

in vec3 fColour;

out vec4 outColour;

void main()
{
    outColour = vec4(fColour, 1.0);
    //outColour *= brightness;
}
