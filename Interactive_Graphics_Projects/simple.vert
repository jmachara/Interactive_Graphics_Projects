#version 330 core

layout(location=1) in vec3 pos;

uniform mat4 mvp_s;
uniform highp float t_l;
uniform int disp;

out highp float t_level;
out mat4 mvp;
out int displacement;

void main()
{
	gl_Position = vec4(pos,1);
	t_level = t_l;
	mvp = mvp_s;
	displacement = disp;
}