#version 330 core

layout(location=4) in vec3 pos;

uniform mat4 vp;

out vec3 dir;

void main()
{
	gl_Position = vp*vec4(pos*10,1);
	dir = pos;
}