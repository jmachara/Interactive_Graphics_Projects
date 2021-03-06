#version 330 core

layout(location=1) in vec3 pos;
//layout(location=1) in vec3 norm;
//layout(location=2) in vec2 txc;


//out vec4 lightView_Position;
out highp float t_level;
out mat4 mvp_out;
out vec4 clr_in;
out vec3 light_dir;
out mat3 normal;
out vec3 view_dir;
out highp float intensity;
out vec3 amb_light;
out highp float a;

uniform highp float t_l;
uniform mat4 mvp;
uniform mat4 shadow_matrix;
uniform mat4 mv;
uniform mat3 norm_mv;
uniform vec3 ks;
uniform vec3 l_dir;
uniform vec3 v_dir;
uniform highp float l_inten;
uniform vec3 amb_l;
uniform highp float alpha;

void main()
{
	gl_Position = vec4(pos.x,pos.y,pos.z,1);
	light_dir = l_dir;
	//normal = normalize(norm_mv*norm);
	mvp_out = mvp;
	clr_in = vec4(.3,.3,.3,1);
	t_level = t_l;
	normal = norm_mv;
	view_dir = v_dir;
	intensity = l_inten;
	amb_light = amb_l;
	a = alpha;
	//lightView_Position = shadow_matrix*vec4(pos,1);
}