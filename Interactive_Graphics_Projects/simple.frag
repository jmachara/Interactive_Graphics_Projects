#version 330 core

layout(location=0) out vec4 color;

uniform sampler2DShadow shadow;

in vec4 lightView_Position;

void main()
{
	color = vec4(.3,.3,.2,1);
	color *= textureProj(shadow,lightView_Position);
}