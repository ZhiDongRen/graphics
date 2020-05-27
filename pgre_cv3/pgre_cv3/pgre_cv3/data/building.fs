#version 130

//Texturing coordinates for diffuse texture
in vec2 coord;

//Diffuse texture of the house
uniform sampler2D tex;

//Shader output
out vec4 fragColor;

void main() 
{
	vec4 color = texture(tex, coord);
	
	fragColor = vec4(color.xyz, 1);
}
