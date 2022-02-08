#version 330 core

layout(location=0) out vec4 color;

in vec3 vColor;
in vec3 light_dir;
in vec3 normal;
in vec3 view_dir;
in highp float intensity;
in vec3 amb_light;
in highp float a;
in vec3 k_s;

void main()
{
	vec4 amb = vec4(amb_light*vColor,1);
	highp float kd_ang = dot(normal,light_dir);
	vec4 kd = vec4(kd_ang,kd_ang,kd_ang,1);
	vec4 ks = vec4(k_s,1);
	vec3 hlf_ang = normalize(light_dir+view_dir);
	vec4 dif = kd*vec4(vColor,1);
	vec4 spec = ks*pow(dot(normalize(normal),hlf_ang),a);

	color = intensity*(dif+spec)+amb;
}