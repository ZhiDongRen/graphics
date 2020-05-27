#version 130

//Input (vertex attributes)
in vec3 position;
in vec2 tc;
in vec2 lm_tc;

//Uniforms
uniform mat4 mvp;

//Output
out vec2 coord;
out vec2 lm_coord;

void main()
{ 
	gl_Position = mvp*vec4(position,1);
	
	//Texture coordinates for tiles
	coord = tc;

	//Texturing coordinates for lightmap
	//Usually the don't differ, but in this case they do
	lm_coord = lm_tc;
}
