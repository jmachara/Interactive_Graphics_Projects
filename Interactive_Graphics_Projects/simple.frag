#version 330 core

layout(location=0) out vec4 color;

uniform sampler2D tex3;

in vec2 texCoord;

void main()
{
	color = texture(tex3,texCoord);
}