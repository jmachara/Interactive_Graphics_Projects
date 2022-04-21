#version 330 core

layout(location=0) out vec4 color;

uniform sampler2D tex;


//uniform samplerCube env;
//uniform sampler2DShadow shadow;

in vec2 txc;
//in vec4 lightView_Position;
in vec3 l_d;
in mat3 norm;
in vec3 v_d;
in highp float i;
in vec3 a_l;
in highp float alpha;
in vec4 clr;

void main()
{
	vec3 normal = norm*texture(tex,txc).rgb;
	//vec4 clr = vec4(.3,.3,.3,1);
	//color = vec4(1,0,0,1);
	//vec4 clr = texture(env,reflect(-view_dir,normal));
	//clr *= textureProj(shadow,lightView_Position);
	vec4 amb = vec4(a_l*clr.xyz,1);
	highp float kd_ang = dot(normal,l_d);
	vec4 kd = vec4(kd_ang,kd_ang,kd_ang,1);
	//vec4 ks = texture(tex2,texCoord);
	vec4 ks = vec4(1,1,1,1);
	vec3 hlf_ang = normalize(l_d+v_d);
	vec4 dif = kd*vec4(clr.xyz,1);
	vec4 spec = ks*pow(dot(normalize(normal),hlf_ang),alpha);
	color = i*(dif+spec)+amb;
}