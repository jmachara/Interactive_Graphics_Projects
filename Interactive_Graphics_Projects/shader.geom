#version 330 core

uniform sampler2D tex2;

layout ( triangles ) in;
layout ( line_strip,max_vertices = 4 ) out;

in mat4 mvp_in[];

void main()
{
	vec2 txc = vec2((gl_in[0].gl_Position.x-10)/-20,(gl_in[0].gl_Position.y-10)/-20);
	gl_Position = mvp_in[0]*(gl_in[0].gl_Position+vec4(0,0,.1,0));
	EmitVertex();
	txc = vec2((gl_in[1].gl_Position.x-10)/-20,(gl_in[1].gl_Position.y-10)/-20);
	gl_Position = mvp_in[1]*(gl_in[1].gl_Position+vec4(0,0,.1,0));
	EmitVertex();
	txc = vec2((gl_in[2].gl_Position.x-10)/-20,(gl_in[2].gl_Position.y-10)/-20);
	gl_Position = mvp_in[2]*(gl_in[2].gl_Position+vec4(0,0,.1,0));
	EmitVertex();
	txc = vec2((gl_in[0].gl_Position.x-10)/-20,(gl_in[0].gl_Position.y-10)/-20);
	gl_Position = mvp_in[0]*(gl_in[0].gl_Position+vec4(0,0,.1,0));
	
	EmitVertex();
}