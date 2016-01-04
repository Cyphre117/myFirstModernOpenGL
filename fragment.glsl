#version 150

uniform float brightness;
uniform sampler2D tex;

in vec3 fColour;
in vec2 fTexCoord;

out vec4 outColour;

void main()
{
    outColour = texture(tex, fTexCoord) * vec4(fColour, 1.0) * brightness;
}
