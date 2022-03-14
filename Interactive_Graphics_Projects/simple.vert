#version 330 core

layout(location=5) in vec3 pos;

uniform mat4 mvp_s;
uniform mat4 shadow_matrix;

out vec4 lightView_Position;

void main()
{
	gl_Position = mvp_s*vec4(pos,1);
	lightView_Position = shadow_matrix*vec4(pos,1);
}