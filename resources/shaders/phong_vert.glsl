#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

out vec3 frag_nor_in;
out vec3 frag_pos;

void main()
{
	gl_Position = P * V * M * vertPos;
	frag_nor_in = (M * vec4(vertNor, 0.0)).xyz;
   frag_pos = (M * vertPos).xyz;
}


