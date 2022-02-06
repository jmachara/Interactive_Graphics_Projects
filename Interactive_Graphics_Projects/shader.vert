#version 330 core

layout(location=0) in vec3 pos;
layout(location=1) in vec3 norm;

out vec3 vColor;
out vec3 light_dir;
out vec3 normal;
out vec3 view_dir;
out highp float intensity;
out vec3 amb_light;
out highp float a;
out vec3 k_s;

uniform mat4 mvp;
uniform mat4 mv;
uniform mat3 norm_mv;
uniform vec3 clr;
uniform vec3 ks;
uniform vec3 l_dir;
uniform vec3 v_dir;
uniform highp float l_inten;
uniform vec3 amb_l;
uniform highp float alpha;

void main()
{
	gl_Position = mvp*vec4(pos,1);
	vColor = clr;
	light_dir = (mv*vec4(l_dir,0)).xyz;
	normal = norm_mv*norm;
	view_dir = (mv*vec4(v_dir,0)).xyz;
	intensity = l_inten;
	amb_light = amb_l;
	k_s = ks;
	a = alpha;
}