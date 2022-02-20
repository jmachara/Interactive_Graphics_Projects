#version 330 core

layout(location=3) in vec3 pos;

uniform mat4 mvp_s;

out vec2 texCoord;

void main()
{
	gl_Position =mvp_s*vec4(pos,1);
	highp float x = (pos.x+10)/20;
	highp float y = (pos.y+10)/20;
	texCoord = vec2(x,y);
}