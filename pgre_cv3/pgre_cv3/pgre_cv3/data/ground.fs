#version 130

//Texturing coordinates for diffuse texture
in vec2 coord;

//Texturing coordinates for lightmap
in vec2 lm_coord;

//Diffuse texture of the house
uniform sampler2D tex;

//Lightmap texture
uniform sampler2D lightMap;

//Shader output
out vec4 fragColor;

void main() 
{
	vec4 color = texture(tex, coord);
	fragColor = vec4(color.xyz, 1);
}
