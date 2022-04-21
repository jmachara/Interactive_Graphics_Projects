#version 330 core

layout ( triangles ) in;
layout ( triangle_strip,max_vertices = 4 ) out;



in vec4 clr_in[];
in vec3 l_dir[];
in mat3 normal[];
in vec3 v_dir[];
in highp float inten[];
in vec3 a_light[];
in highp float a[];
in vec2 txCoord[];
in mat4 mvp_out[];

out vec4 clr;
out vec3 l_d;
out mat3 norm;
out vec3 v_d;
out highp float i;
out vec3 a_l;
out highp float alpha;
out vec2 txc;

void main()
{
	clr = clr_in[0];
	l_d = l_dir[0];
	norm = normal[0];
	v_d = v_dir[0];
	i = inten[0];
	a_l = a_light[0];
	alpha = a[0];
	
	txc = vec2((gl_in[0].gl_Position.x-10)/-20,(gl_in[0].gl_Position.y-10)/-20);
	gl_Position = mvp_out[0]*gl_in[0].gl_Position;
	EmitVertex();
	txc = vec2((gl_in[1].gl_Position.x-10)/-20,(gl_in[1].gl_Position.y-10)/-20);
	gl_Position = mvp_out[1]*gl_in[1].gl_Position;
	EmitVertex();
	txc = vec2((gl_in[2].gl_Position.x-10)/-20,(gl_in[2].gl_Position.y-10)/-20);
	gl_Position = mvp_out[2]*gl_in[2].gl_Position;	
	EmitVertex();
	txc = vec2((gl_in[0].gl_Position.x-10)/-20,(gl_in[0].gl_Position.y-10)/-20);
	gl_Position = mvp_out[0]*gl_in[0].gl_Position;
	EmitVertex();
}