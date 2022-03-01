#version 330 core

layout(location=0) out vec4 color;

uniform samplerCube env;

in vec3 dir;

void main()
{
	color = texture(env,dir);
}