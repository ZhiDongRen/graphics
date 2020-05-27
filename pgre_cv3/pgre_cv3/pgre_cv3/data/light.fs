#version 430

uniform vec3 lightAmbientColor;
uniform vec3 lightDiffuseColor;

out vec4 fragColor;

void main()
{
    fragColor = vec4(lightAmbientColor + lightDiffuseColor, 1);
}