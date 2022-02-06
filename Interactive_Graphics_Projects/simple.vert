#version 330 core

layout(location=0) in vec3 pos;

out vec3 vColor;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp*vec4(pos,1);
	vColor = vec3(.01,.7,.5);
}