#version 330 core

layout(location=0) out vec4 color;

uniform sampler2D tex;
uniform sampler2D tex2;
in vec2 texCoord;

in vec3 light_dir;
in vec3 normal;
in vec3 view_dir;
in highp float intensity;
in vec3 amb_light;
in highp float a;


void main()
{
	vec4 clr = texture(tex,texCoord);
	vec4 amb = vec4(amb_light*clr.xyz,1);
	highp float kd_ang = dot(normal,light_dir);
	vec4 kd = vec4(kd_ang,kd_ang,kd_ang,1);
	vec4 ks = texture(tex2,texCoord);
	vec3 hlf_ang = normalize(light_dir+view_dir);
	vec4 dif = kd*vec4(clr.xyz,1);
	vec4 spec = ks*pow(dot(normalize(normal),hlf_ang),a);

	color = intensity*(dif+spec)+amb;
}