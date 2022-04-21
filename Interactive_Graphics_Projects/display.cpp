#include <GL/glew.h>
#include <GL/freeglut.h>
#include "cyCodeBase/cyTriMesh.h"
#include "cyCodeBase/cyMatrix.h"
#include <stdio.h>
#include <string>
#include<fstream>
#include<iostream>
#include "cyCodeBase/cyGL.h"
#include "lodepng/lodepng.h"

//glut functions
void display_function();
void keyboard_function(unsigned char key, int x, int y);
void keyboard_up_function(unsigned char key, int x, int y);
void special_k_function(int key, int x, int y);
void special_k_up_function(int key, int x, int y);
void mouse_func(int x, int y);
void mouse_click_func(int button, int state, int x, int y);
void mouse_passive_func(int x, int y);
void idle_function();
void wind_reshape(int x, int y);
//helper methods
cy::Matrix4f create_mv(float mid);
cy::Matrix3f create_norm_mv(float mid);
cy::Matrix4f create_mvp(float mid,double rot_x,double rot_z,float distance);
cy::Matrix4f create_mlp(float mid, double rot_x, double rot_z, float distance);
cy::Matrix4f create_cube_mvp(float mid, double rot_x, double rot_z, float distance);
std::vector<cy::Vec3f> build_triangle_buff(cyTriMesh mesh);
std::vector<cy::Vec3f> build_cube_triangle_buff(cyTriMesh mesh);
std::vector<cy::Vec3f> build_square_buff();
std::vector<cy::Vec3f> build_square_triangle_buff();
std::vector<cy::Vec3f> build_plain_triangle_buff();
std::vector<cy::Vec3f> build_norm_buff(cyTriMesh mesh);
std::vector<cy::Vec2f> build_texture_buff(cyTriMesh mesh);
void render_texture();
void make_cube_map();
void render_object();
void render_image();
void render_plain();
void geom_shade();
void render_cube();
void render_shadow_map();
void set_textures();
void set_uniforms();
void set_vao();
//bg color
double red;
double blue;
double green;
double alpha;
//texture
cyTriMesh mesh = cyTriMesh();
cyTriMesh cube = cyTriMesh();
//vao
GLuint vao;
//pot rotation
float mid;
double rotx = 0;// -1.5;
double rotz = 0;// -2.5;
double erotx = 0;
double erotz = 0;
//plain rotation
double p_rotx = -1.5;
double p_rotz = -2.5;
//camera
cy::Vec3f cam_dir;
float dist = 50.0;
float dist_holder = 50.0;
float fov = .7;
float cam_height = 100;
//light
cy::Vec3f light_pos;
float light_intensity;
float amb = .3;
float light_hori = 0;
float light_vert = 0;
cy::Matrix3f l_mv;
//material
float a;
float bias;
cyGLTexture2D tex, tex2;
cy::GLRenderTexture2D render_buff;
cy::GLRenderDepth2D shadowMap;
//program variables
cy::GLSLProgram prog, prog2, prog3, prog_shadow;
int buff_size;
int cube_buff_size;
int square_buff;
int square_buff2;
int norm_buff_size;
int width = 1920;
int height = 1080;
bool geom = false;
int mouse_x;
int mouse_y;
int tl = 1;
bool ctrl = false;
bool alt = false;
bool rc = false;
bool first = true;
int disp = 0;
int rc_click;
//environment
cy::GLTextureCubeMap envMap;

//main
int main(int argc, char** argv)
{
	//Initialize glut
	glutInit(&argc, argv);


	//window
	glutInitContextVersion(4, 5);
	glutInitContextFlags(GLUT_DEBUG);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("Jack's GLUT");

	//glew
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	//program

	prog.BuildFiles("shader.vert", "shader.frag","shader2.geom","shader2.tsl","shader_eval2.tsl");
	prog2.BuildFiles("simple.vert", "simple.frag", "shader.geom", "shader.tsl", "shader_eval.tsl");

	//prog3.BuildFiles("env.vert", "env.frag");
	//prog_shadow.BuildFiles("shadow.vert", "shadow.frag");
	//vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	//mesh.LoadFromFileObj(argv[1]);
	//cube.LoadFromFileObj("cube.obj");


	//mesh.ComputeBoundingBox();
	//while (!mesh.IsBoundBoxReady()) {}
	//cy::Vec3f low_bound = mesh.GetBoundMin();
	//cy::Vec3f high_bound = mesh.GetBoundMax();
	//mid = (high_bound[2] - low_bound[2])/2;

	

	//texture
	//set_textures();
	tex.Initialize();
	std::vector<unsigned char> image;
	unsigned tex_w, tex_h;
	lodepng::decode(image, tex_w, tex_h, argv[1]);
	tex.SetImage(&image.front(), 4, tex_w, tex_h, 0);
	if (argc == 3)
	{	
		disp = 1;
		image.clear();
		tex2.Initialize();
		lodepng::decode(image, tex_w, tex_h, argv[2]);
		tex2.SetImage(&image.front(), 4, tex_w, tex_h, 0);
	}
	

	//uniforms
	light_pos = cy::Vec3f(cos(light_hori) * dist, cam_height, sin(light_hori) * dist);
	light_intensity = 1;
	a = 50;
	bias = .00003;
	//vao and vbo
	set_vao();
	//callback
	glutDisplayFunc(display_function);
	glutKeyboardFunc(keyboard_function);
	glutKeyboardUpFunc(keyboard_up_function);
	//glutIdleFunc(idle_function);
	glutMotionFunc(mouse_func);
	glutMouseFunc(mouse_click_func);
	//glutPassiveMotionFunc(mouse_passive_func);
	glutSpecialFunc(special_k_function);
	glutReshapeFunc(wind_reshape);
	glutSpecialUpFunc(special_k_up_function);
	set_uniforms();
	//opengl
	red = 0.3;
	green = 0.3;
	blue = 0.3;
	alpha = 1;
	glClearColor(0,0,0, alpha);
	//call glut
	glutMainLoop();
	return 0;
}
//helper functions
cy::Matrix4f create_mvp(float mid,double rot_x, double rot_z,float distance)
{
	cy::Vec3f cam_pos = { 0,0,distance};
	cy::Vec3f target = { 0,0,0 };
	cam_dir = (cam_pos - target).GetNormalized();
	cy::Vec3f up = { 0,1,0 };
	cy::Vec3f cam_r = up.Cross(cam_dir);
	cy::Vec3f cam_u = cam_dir.Cross(cam_r);
	cy::Matrix4f view = cy::Matrix4f::View(cam_pos, target, cam_u);
	cy::Matrix4f trans = cy::Matrix4f::Translation({ 0,0,-mid });
	cy::Matrix4f rtrans = cy::Matrix4f::Translation({ 0,0,mid });
	cy::Matrix3f xRot = cy::Matrix3f::RotationX(rot_x);
	cy::Matrix3f zRot = cy::Matrix3f::RotationY(rot_z);
	cy::Matrix4f rot = rtrans*xRot*zRot*trans;
	cy::Matrix4f proj = cy::Matrix4f::Perspective(fov, float(width) / float(height), .1f, 1000.0f);

	return proj*view*rot;
}
cy::Matrix4f create_mlp(float mid, double rot_x, double rot_z, float distance)
{
	cy::Vec3f cam_pos = light_pos;
	cy::Vec3f target = { 0,0,0 };
	cam_dir = (cam_pos - target).GetNormalized();
	cy::Vec3f up = { 0,1,0 };
	cy::Vec3f cam_r = up.Cross(cam_dir);
	cy::Vec3f cam_u = cam_dir.Cross(cam_r);
	cy::Matrix4f view = cy::Matrix4f::View(cam_pos, target, cam_u);
	cy::Matrix4f trans = cy::Matrix4f::Translation({ 0,0,-mid });
	cy::Matrix4f rtrans = cy::Matrix4f::Translation({ 0,0,mid });
	cy::Matrix3f xRot = cy::Matrix3f::RotationX(rot_x);
	cy::Matrix3f zRot = cy::Matrix3f::RotationZ(rot_z);
	cy::Matrix4f rot = rtrans * xRot * zRot * trans;
	cy::Matrix4f proj = cy::Matrix4f::Perspective(fov, float(width) / float(height), .1f, 1000.0f);

	return proj * view * rot;
}
cy::Matrix4f create_cube_mvp(float mid, double rot_x, double rot_y, float distance)
{
	cy::Vec3f cam_pos = { 0,0,distance };
	cy::Vec3f target = { 0,0,0 };
	cy::Vec3f cam_u = { 0,1,0 };
	cy::Matrix4f view = cy::Matrix4f(cy::Matrix3f(cy::Matrix4f::View(cam_pos, target, cam_u)));
	cy::Matrix3f xRot = cy::Matrix3f::RotationX(rot_x);
	cy::Matrix3f zRot = cy::Matrix3f::RotationY(rot_y);
	cy::Matrix3f rot = xRot * zRot;
	cy::Matrix4f proj = cy::Matrix4f::Perspective(fov, float(width) / float(height), .1f, 1000.0f);

	return proj * view * rot;
}
cy::Matrix4f create_mv(float mid)
{
	cy::Vec3f cam_pos = { 0,0,dist};
	cy::Vec3f target = { 0,0,0 };
	cy::Vec3f cam_dir = (cam_pos - target).GetNormalized();
	cy::Vec3f up = { 0,1,0 };
	cy::Vec3f cam_r = up.Cross(cam_dir);
	cy::Vec3f cam_u = cam_dir.Cross(cam_r);
	cy::Matrix4f trans = cy::Matrix4f::Translation({ 0,0,-mid });
	cy::Matrix4f rtrans = cy::Matrix4f::Translation({ 0,0,mid });
	cy::Matrix3f xRot = cy::Matrix3f::RotationX(rotx);
	cy::Matrix3f zRot = cy::Matrix3f::RotationY(rotz);
	cy::Matrix4f view = cy::Matrix4f::View(cam_pos, target, cam_u);
	cy::Matrix4f rot = rtrans * xRot * zRot * trans;
	cy::Matrix4f mv = view * rot;
	return mv;

}
cy::Matrix3f create_norm_mv(float mid)
{
	cy::Vec3f cam_pos = { 0,0,dist };
	cy::Vec3f target = { 0,0,0 };
	cy::Vec3f cam_dir = (cam_pos - target).GetNormalized();
	cy::Vec3f up = { 0,1,0 };
	cy::Vec3f cam_r = up.Cross(cam_dir);
	cy::Vec3f cam_u = cam_dir.Cross(cam_r);
	cy::Matrix4f trans = cy::Matrix4f::Translation({ 0,0,-mid });
	cy::Matrix4f rtrans = cy::Matrix4f::Translation({ 0,0,mid });
	cy::Matrix3f xRot = cy::Matrix3f::RotationX(rotx);
	cy::Matrix3f zRot = cy::Matrix3f::RotationY(rotz);
	cy::Matrix4f view = cy::Matrix4f::View(cam_pos, target, cam_u);
	cy::Matrix4f rot = rtrans * xRot * zRot * trans;
	cy::Matrix4f mv = (view * rot).GetInverse().GetTranspose();
	return mv.GetSubMatrix3();
}
std::vector<cy::Vec3f> build_triangle_buff(cyTriMesh mesh)
{
	int tri_points = mesh.NF();
	std::vector<cy::Vec3f> triangles;
	buff_size = 3 * tri_points;
	for (int i = 0; i < tri_points; i++)
	{
		cy::TriMesh::TriFace face = mesh.F(i);
		triangles.push_back(mesh.V(face.v[0]));
		triangles.push_back(mesh.V(face.v[1]));
		triangles.push_back(mesh.V(face.v[2]));
	}
	return triangles;
}
std::vector<cy::Vec3f> build_cube_triangle_buff(cyTriMesh mesh)
{
	int tri_points = mesh.NF();
	std::vector<cy::Vec3f> triangles;
	cube_buff_size = 3 * tri_points;
	for (int i = 0; i < tri_points; i++)
	{
		cy::TriMesh::TriFace face = mesh.F(i);
		triangles.push_back(mesh.V(face.v[0]));
		triangles.push_back(mesh.V(face.v[1]));
		triangles.push_back(mesh.V(face.v[2]));
	}
	return triangles;
}
std::vector<cy::Vec3f> build_square_buff()
{
	std::vector<cy::Vec3f> triangles;
	double inc = 20;
	double i = -10;
	double j = -10;
	square_buff = (4);
	triangles.push_back(cy::Vec3f(i, j, 0));
	triangles.push_back(cy::Vec3f(i, j + inc, 0));
	triangles.push_back(cy::Vec3f(i + inc, j + inc, 0));
	triangles.push_back(cy::Vec3f(i + inc, j, 0));
	
	return triangles;
}
std::vector<cy::Vec3f> build_square_triangle_buff()
{
	std::vector<cy::Vec3f> triangles;
	double inc = 20;
	double i = -10;
	double j = -10;
	square_buff2 = (6);
	triangles.push_back(cy::Vec3f(i, j, 0));
	triangles.push_back(cy::Vec3f(i + inc, j, 0));
	triangles.push_back(cy::Vec3f(i, j + inc, 0));
	triangles.push_back(cy::Vec3f(i + inc, j, 0));
	triangles.push_back(cy::Vec3f(i, j + inc, 0));
	triangles.push_back(cy::Vec3f(i + inc, j + inc, 0));
	return triangles;
}
std::vector<cy::Vec3f> build_plain_triangle_buff()
{
	std::vector<cy::Vec3f> triangles;
	double i = -40;
	double j = -40;
	square_buff = (6);
	triangles.push_back(cy::Vec3f(i, j, 0));
	triangles.push_back(cy::Vec3f(i + 80, j, 0));
	triangles.push_back(cy::Vec3f(i, j + 80, 0));
	triangles.push_back(cy::Vec3f(i + 80, j, 0));
	triangles.push_back(cy::Vec3f(i, j + 80, 0));
	triangles.push_back(cy::Vec3f(i + 80, j + 80, 0));
	return triangles;
}
std::vector<cy::Vec3f> build_norm_buff(cyTriMesh mesh)
{
	int faces = mesh.NF();
	std::vector<cy::Vec3f> norms;
	norm_buff_size = 3 * faces;
	for(int i = 0; i < faces; i++)
	{
		cy::TriMesh::TriFace face = mesh.F(i);
		norms.push_back(mesh.VN(face.v[0]));
		norms.push_back(mesh.VN(face.v[1]));
		norms.push_back(mesh.VN(face.v[2]));
	}
	
	return norms;
}
std::vector<cy::Vec2f> build_texture_buff(cyTriMesh mesh)
{
	int faces = mesh.NF();
	std::vector<cy::Vec2f> coords;
	norm_buff_size = 3 * faces;
	for (int i = 0; i < faces; i++)
	{
		cy::TriMesh::TriFace face = mesh.FT(i);
		cy::Vec3f texture1 = mesh.VT(face.v[0]);
		cy::Vec3f texture2 = mesh.VT(face.v[1]);
		cy::Vec3f texture3 = mesh.VT(face.v[2]);
		coords.push_back(texture1.XY());
		coords.push_back(texture2.XY());
		coords.push_back(texture3.XY());
	}

	return coords;
}
void render_texture()
{
	prog.Bind();
	render_buff.Initialize(true, 3, width,height);
	render_buff.Bind();
	tex.Bind(0);
	tex2.Bind(1);
	glClearColor(red, green, blue, alpha);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, buff_size);
	render_buff.Unbind();
	render_buff.BuildTextureMipmaps();
	render_buff.SetTextureAnisotropy(4);
	render_buff.SetTextureFilteringMode(GL_LINEAR_MIPMAP_LINEAR,GL_LINEAR_MIPMAP_LINEAR);
}
void render_shadow_map()
{
	prog_shadow.Bind();
	shadowMap.Initialize(true, width, height);
	shadowMap.SetTextureFilteringMode(GL_LINEAR, GL_LINEAR);
	shadowMap.Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, buff_size);
	shadowMap.Unbind();
}
void render_object()
{
	prog.Bind();
	shadowMap.BindTexture(2);
	//tex.Bind(0);
	//tex2.Bind(1);
	glDrawArrays(GL_TRIANGLES, 0, buff_size);
}
void render_image() 
{
	prog2.Bind();
	//render_buff.BindTexture(2);
	shadowMap.BindTexture(2);
	glDrawArrays(GL_TRIANGLES, 0, square_buff);
}
void render_plain()
{
	prog.Bind();
	tex.Bind(0);
 	tex2.Bind(1);
	//shadowMap.BindTexture(2);
	glDrawArrays(GL_PATCHES, 0, square_buff);
	
}
void geom_shade()
{
	prog2.Bind();
	tex2.Bind(0);
	//glDrawArrays(GL_TRIANGLES, 0, square_buff);
	glDrawArrays(GL_PATCHES, 0, square_buff);
}
void render_cube()
{
	prog3.Bind();
	envMap.Bind(3);
	glDrawArrays(GL_TRIANGLES, 0, cube_buff_size);

}
void make_cube_map()
{
	envMap.Initialize();
	std::vector<unsigned char> image;
	unsigned tex_w, tex_h;
	lodepng::decode(image, tex_w, tex_h, "cubemap_posx.png");
	envMap.SetImageRGBA((cy::GLTextureCubeMap::Side) 0, &image.front(), tex_w, tex_h);
	image.clear();
	lodepng::decode(image, tex_w, tex_h, "cubemap_negx.png");
	envMap.SetImageRGBA((cy::GLTextureCubeMap::Side) 1, &image.front(), tex_w, tex_h);
	image.clear();
	lodepng::decode(image, tex_w, tex_h, "cubemap_posy.png");
	envMap.SetImageRGBA((cy::GLTextureCubeMap::Side) 2, &image.front(), tex_w, tex_h);
	image.clear();
	lodepng::decode(image, tex_w, tex_h, "cubemap_negy.png");
	envMap.SetImageRGBA((cy::GLTextureCubeMap::Side) 3, &image.front(), tex_w, tex_h);
	image.clear();
	lodepng::decode(image, tex_w, tex_h, "cubemap_posz.png");
	envMap.SetImageRGBA((cy::GLTextureCubeMap::Side) 4, &image.front(), tex_w, tex_h);
	image.clear();
	lodepng::decode(image, tex_w, tex_h, "cubemap_negz.png");
	envMap.SetImageRGBA((cy::GLTextureCubeMap::Side) 5, &image.front(), tex_w, tex_h);
	envMap.BuildMipmaps();
	envMap.SetSeamless();
	
}
void set_textures()
{
	tex.Initialize();
	std::vector<unsigned char> image, image2;
	unsigned tex_w, tex_h,ks_tex_w,ks_tex_h;
	//cy::TriMesh::Mtl material = mesh.M(0);
	//lodepng::decode(image, tex_w, tex_h, material.map_Kd.data);
	/*tex.SetImage(&image.front(), 4, tex_w, tex_h, 0);
	tex.BuildMipmaps();
	tex2.Initialize();
	lodepng::decode(image2, ks_tex_w, ks_tex_h, material.map_Ks.data);
	tex2.SetImage(&image2.front(), 4, ks_tex_w, ks_tex_h, 0);
	tex2.BuildMipmaps();
	make_cube_map();*/
}
void set_uniforms()
{
	//cy::Matrix4f mvp = create_mvp(mid,rotx,rotz,dist);
	cy::Matrix4f mvp = create_mvp(0, rotx, rotz, dist);
	cy::Matrix3f norm_mv = create_norm_mv(0);
	cy::Matrix4f mv = create_mv(0);
	light_pos = cy::Vec3f(cos(light_hori)*dist, cam_height, sin(light_hori)*dist);
	prog["mvp"] = mvp;
	prog["mv"] = mv;
	prog["norm_mv"] = norm_mv;
	prog["l_dir"] = (light_pos - cy::Vec3f(0, 0, 0)).GetNormalized();
	prog["v_dir"] = cam_dir;
	prog["l_inten"] = light_intensity;
	prog["amb_l"] = cy::Vec3f(amb, amb, amb);
	prog["alpha"] = a;
	prog["tex"] = 0;	
	prog["tex2"] = 1;
	prog["samplerCube"] = 3;
	prog["shadow_matrix"] = cy::Matrix4f::Translation(cy::Vec3f(.5, .5, .5-bias)) * cy::Matrix4f::Scale(.5) * create_mlp(mid, rotx, rotz, dist);
	prog["shadow"] = 2;
	prog["t_l"] = float(tl);

	//simple
	prog2["mvp_s"] = create_mvp(0, rotx, rotz, dist);
	prog2["t_l"] = float(tl);
	prog2["tex2"] = 0;
	prog2["disp"] = disp;
	//prog2["shadow"] = 2;
	//prog2["shadow_matrix"] = cy::Matrix4f::Translation(cy::Vec3f(.5, .5, .5-bias))* cy::Matrix4f::Scale(.5) * create_mlp(mid, rotx, rotz, dist);
	//env
	prog3["samplerCube"] = 3;
	prog3["vp"] = create_cube_mvp(0, erotx, erotz, 1);
	//shadow
	cy::Matrix4f mlp = create_mlp(mid, rotx, rotz, dist);
	prog_shadow["mlp"] = mlp;

}
void display_function()
{
	//render_texture();
	//render_shadow_map();
	glClearColor(0, 0, 0, alpha);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	//glDisable(GL_DEPTH_TEST);
	//render_cube();
	render_plain();
	if (geom)
	{
		geom_shade();
	}
	//render_object();
	//render_image();
	glutSwapBuffers();
}
//glut functions
void mouse_click_func(int button, int state, int x, int y)
{
	if (button == 0)
	{
		mouse_x = x;
		mouse_y = y;
	}
	else
	{
		rc = !rc;
		mouse_x = x;
	}
	glutPostRedisplay();

}
void mouse_passive_func(int x, int y)
{
}
void mouse_func(int x, int y)
{
	if (rc)
	{
		dist_holder -= (x - mouse_x) / 2;
		dist = abs(dist_holder);
	}
	/*else if (alt)
	{
		if (x - mouse_x != 0)
			p_rotz += ((x - mouse_x)) * 3.14 / 180;
		if (y - mouse_y != 0)
			p_rotx += ((y - mouse_y) * 3.14) / 180;

	}*/
	else if (ctrl)
	{
		if (x - mouse_x != 0)
			light_hori += ((x - mouse_x)) * 3.14 / 180;
		if (y - mouse_y != 0)
			cam_height -= (y - mouse_y);

	}
	
	else
	{
		if (x - mouse_x != 0)
		{		
			rotz += ((x - mouse_x)) * 3.14 / 180;
			erotz += ((x - mouse_x)) * 3.14 / 180;
			p_rotz += ((x - mouse_x)) * 3.14 / 180;
		}
		if (y - mouse_y != 0)
		{
			rotx += ((y - mouse_y) * 3.14) / 180;
			erotx += ((y - mouse_y) * 3.14) / 180;
			p_rotx += ((y - mouse_y) * 3.14) / 180;

		}
	}
	set_uniforms();
	mouse_y = y;
	mouse_x = x;
	glutPostRedisplay();
}
void idle_function()
{
	
}
void keyboard_function(unsigned char key, int x, int y)
{
	switch (key) {
	case 27://esc
		glutLeaveMainLoop();
		break;
	case 32: 
		geom = !geom;
		break;
	case 112://p
		rotx = -1.5;
       	rotz = -2.5;
		erotx = -1.5;
		erotz = -2.5;
		dist = 100.0;
		set_uniforms();
		break;
	}
	glutPostRedisplay();
	
}
void keyboard_up_function(unsigned char key, int x, int y)
{
	glutPostRedisplay();
}
void special_k_function(int key, int x, int y)
{
	switch (key) {
	case 9://f9
		prog.BuildFiles("shader.vert", "shader.frag");
		set_uniforms();
		break;
	case 100:
		if (tl > 1)
			tl -= 1;
		break;
	case 102:
		if(tl < 50	)
			tl += 1;
		break;
	case 114://ctrl
		ctrl = true;
		break;
	case 116://alt
		alt = true;
		break;
	}
	set_uniforms();
	glutPostRedisplay();
}
void wind_reshape(int x, int y)
{
	width = x;
	height = y;
	set_uniforms();
	glViewport(0, 0, width, height);
	set_uniforms();
	glutPostRedisplay();
}
void special_k_up_function(int key, int x, int y)
{
	switch (key) {
	case 114://ctrl
		ctrl = false;
		break;
	case 116://alt
		alt = false;
		break;
	}
	glutPostRedisplay();
}
void set_vao()
{
//	//triangle buffer 0 
//	GLuint triangle_buff;
//	glGenBuffers(1, &triangle_buff);
//	std::vector<cy::Vec3f> tri_buff = build_triangle_buff(mesh);
//	glBindBuffer(GL_ARRAY_BUFFER, triangle_buff);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * mesh.NF() * 3, &tri_buff.front(), GL_STATIC_DRAW);
//	//norm buffer 1
//	GLuint norm_buff;
//	glGenBuffers(1, &norm_buff);
//	std::vector<cy::Vec3f> n_buff = build_norm_buff(mesh);
//	glBindBuffer(GL_ARRAY_BUFFER, norm_buff);
//	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(cy::Vec3f) * mesh.NF(), &n_buff.front(), GL_STATIC_DRAW);
//	//texture buffer 2
//	GLuint tex_buff;
//	glGenBuffers(1, &tex_buff);
//	std::vector<cy::Vec2f> t_buff = build_texture_buff(mesh);
//	glBindBuffer(GL_ARRAY_BUFFER, tex_buff);
//	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(cy::Vec2f) * mesh.NF(), &t_buff.front(), GL_STATIC_DRAW);

	//plain buffer 3
	GLuint plain_buff;
	glGenBuffers(1, &plain_buff);
	std::vector<cy::Vec3f> s_buff = build_square_buff();
	glBindBuffer(GL_ARRAY_BUFFER, plain_buff);
	glBufferData(GL_ARRAY_BUFFER, square_buff * sizeof(cy::Vec3f), &s_buff.front(), GL_STATIC_DRAW);
	
	GLuint plain_buff2;
	glGenBuffers(1, &plain_buff2);
	std::vector<cy::Vec3f> s_buff2 = build_square_triangle_buff();
	glBindBuffer(GL_ARRAY_BUFFER, plain_buff2);
	glBufferData(GL_ARRAY_BUFFER, square_buff2 * sizeof(cy::Vec3f), &s_buff2.front(), GL_STATIC_DRAW);
	////cube buffer 4 
	//GLuint cube_buff;
	//glGenBuffers(1, &cube_buff);
	//std::vector<cy::Vec3f> c_buff = build_cube_triangle_buff(cube);
	//glBindBuffer(GL_ARRAY_BUFFER, cube_buff);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * cube.NF() * 3, &c_buff.front(), GL_STATIC_DRAW);
	////plain buffer 5
	//GLuint plain_buff2;
	//glGenBuffers(1, &plain_buff2);
	//std::vector<cy::Vec3f> p_buff = build_plain_triangle_buff();
	//glBindBuffer(GL_ARRAY_BUFFER, plain_buff2);
	//glBufferData(GL_ARRAY_BUFFER, square_buff * sizeof(cy::Vec3f), &p_buff.front(), GL_STATIC_DRAW);

	//glVertexArrayVertexBuffer(vao, 0, triangle_buff, 0, sizeof(cy::Vec3f));
	//glVertexArrayAttribBinding(vao, 0, 0);
	//glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
	//glVertexArrayBindingDivisor(vao, 0, 0);
	//glEnableVertexArrayAttrib(vao, 0);

	//glVertexArrayVertexBuffer(vao, 1, norm_buff, 0, sizeof(cy::Vec3f));
	//glVertexArrayAttribBinding(vao, 1, 1);
	//glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, 0);
	//glVertexArrayBindingDivisor(vao, 1, 0);
	//glEnableVertexArrayAttrib(vao, 1);

	//glVertexArrayVertexBuffer(vao, 2, tex_buff, 0, sizeof(cy::Vec2f));
	//glVertexArrayAttribBinding(vao, 2, 2);
	//glVertexArrayAttribFormat(vao, 2, 3, GL_FLOAT, GL_FALSE, 0);
	//glVertexArrayBindingDivisor(vao, 2, 0);
	//glEnableVertexArrayAttrib(vao, 2);
	glVertexArrayVertexBuffer(vao, 0, plain_buff2, 0, sizeof(cy::Vec3f));
	glVertexArrayAttribBinding(vao, 0, 0);
	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayBindingDivisor(vao, 0, 0);
	glEnableVertexArrayAttrib(vao, 0);

	glVertexArrayVertexBuffer(vao, 1, plain_buff, 0, sizeof(cy::Vec3f));
	glVertexArrayAttribBinding(vao, 1, 1);
	glVertexArrayAttribFormat(vao,1, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayBindingDivisor(vao, 1, 0);
	glEnableVertexArrayAttrib(vao, 1);


//	glVertexArrayVertexBuffer(vao, 4, cube_buff, 0, sizeof(cy::Vec3f));
//	glVertexArrayAttribBinding(vao, 4, 4);
//	glVertexArrayAttribFormat(vao, 4, 3, GL_FLOAT, GL_FALSE, 0);
//	glVertexArrayBindingDivisor(vao, 4, 0);
//	glEnableVertexArrayAttrib(vao, 4);
//
//	glVertexArrayVertexBuffer(vao, 5, plain_buff2, 0, sizeof(cy::Vec3f));
//	glVertexArrayAttribBinding(vao, 5, 5);
//	glVertexArrayAttribFormat(vao, 5, 3, GL_FLOAT, GL_FALSE, 0);
//	glVertexArrayBindingDivisor(vao, 5, 0);
//	glEnableVertexArrayAttrib(vao, 5);
}
