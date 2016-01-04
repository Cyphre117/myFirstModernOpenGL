#version 150

in vec2 position;
in vec3 vColour;
in vec2 vTexCoord;

out vec3 fColour;
out vec2 fTexCoord;

void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
    fColour = vColour;
    fTexCoord = vTexCoord;
}
