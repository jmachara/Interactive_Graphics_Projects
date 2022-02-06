#include <GL/glew.h>
#include <GL/freeglut.h>
#include "cyCodeBase/cyTriMesh.h"
#include "cyCodeBase/cyMatrix.h"
#include <stdio.h>
#include <string>
#include<fstream>
#include<iostream>
#include "cyCodeBase/cyGL.h"

//glut functions
void display_function();
void keyboard_function(unsigned char key, int x, int y);
void mouse_func(int x, int y);
void mouse_click_func(int button, int state, int x, int y);
void mouse_passive_func(int x, int y);
void idle_function();
void wind_reshape();
//helper methods
cy::Matrix4f create_mv(float mid);
cy::Matrix3f create_norm_mv(float mid);
cy::Matrix4f create_mvp(float mid);
std::vector<cy::Vec3f> build_triangle_buff(cyTriMesh mesh);
std::vector<cy::Vec3f> build_norm_buff(cyTriMesh mesh);
//bg color
double red;
double blue;
double green;
double alpha;
//pot color
double pot_r;
double pot_g;
double pot_b;
//camera
cy::Vec3f cam_dir;
//light
cy::Vec3f light_pos;
float light_intensity;
float light_vert = 100;
float amb = .4;

//material
float a;
float ks = 1;
//rotation
float mid;
//Variable declarations
cy::GLSLProgram prog;
int buff_size;
int norm_buff_size;
int width = 1920;
int height = 1080;
double rotx = -1.5;
double rotz = 0;
float dist = 100.0;
int mouse_x;
int mouse_y;
bool rc = false;
int rc_click;
double c;

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
	glutCreateWindow("Teapot");

	//glew
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	//program
	prog.Bind();
	//vao
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	//vbo
	cyTriMesh mesh = cyTriMesh();
	mesh.LoadFromFileObj(argv[1]);
	mesh.ComputeBoundingBox();
	while (!mesh.IsBoundBoxReady()) {}
	cy::Vec3f low_bound = mesh.GetBoundMin();
	cy::Vec3f high_bound = mesh.GetBoundMax();
	mid = (high_bound[2] - low_bound[2])/2;
	//triangle buffer
	GLuint triangle_buff;
	glGenBuffers(1, &triangle_buff);
	std::vector<cy::Vec3f> tri_buff = build_triangle_buff(mesh);
	glBindBuffer(GL_ARRAY_BUFFER, triangle_buff);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cy::Vec3f) * mesh.NF() * 3, &tri_buff.front(), GL_STATIC_DRAW);
	//norm buffer
	GLuint norm_buff;
	glGenBuffers(1, &norm_buff);
	std::vector<cy::Vec3f> n_buff = build_norm_buff(mesh);
	glBindBuffer(GL_ARRAY_BUFFER, norm_buff);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(cy::Vec3f) * mesh.NF(), &n_buff.front(), GL_STATIC_DRAW);
	
	//vs and fs
	prog.BuildFiles("shader.vert", "shader.frag");
	//uniform
	cy::Matrix4f mvp = create_mvp(mid);
	cy::Matrix3f norm_mv = create_norm_mv(mid);
	cy::Matrix4f mv = create_mv(mid);
	light_pos = cy::Vec3f(dist, 0 , light_vert);
	light_intensity = 1;
	a = 5;
	pot_r = 1;
	pot_g = 0;
	pot_b = 0;
	prog["mvp"] = mvp;
	prog["mv"] = mv;
	prog["norm_mv"] = norm_mv;
	prog["clr"] = cy::Vec3f(pot_r,pot_g,pot_b);
	prog["l_dir"] = (light_pos-cy::Vec3f(0,0,0)).GetNormalized();
	prog["v_dir"] = cam_dir;
	prog["l_inten"] = light_intensity;
	prog["amb_l"] = cy::Vec3f(amb,amb,amb);
	prog["alpha"] = a;
	prog["ks"] = cy::Vec3f(ks, ks, ks);
	//vao and vbo
	glVertexArrayVertexBuffer(vao, 0, triangle_buff, 0, sizeof(cy::Vec3f));
	glVertexArrayAttribBinding(vao, 0, 0);
	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(cy::Vec3f));
	glVertexArrayBindingDivisor(vao, 0, 0);
	glEnableVertexArrayAttrib(vao, 0);
	glVertexArrayVertexBuffer(vao, 1, norm_buff, 0, sizeof(cy::Vec3f));
	glVertexArrayAttribBinding(vao, 1, 1);
	glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(cy::Vec3f));
	glVertexArrayBindingDivisor(vao, 1, 0);
	glEnableVertexArrayAttrib(vao, 1);

	//callback
	glutDisplayFunc(display_function);
	glutKeyboardFunc(keyboard_function);
	//glutIdleFunc(idle_function);
	glutMotionFunc(mouse_func);
	glutMouseFunc(mouse_click_func);
	//glutPassiveMotionFunc(mouse_passive_func);

	//opengl
	red = 0;
	green = 0;
	blue = 0;
	alpha = 1;
	glClearColor(red, green, blue, alpha);

	//call glut
	glutMainLoop();
	return 0;
}
cy::Matrix4f create_mvp(float mid)
{
	cy::Vec3f cam_pos = { 0,0,dist};
	cy::Vec3f target = { 0,0,0 };
	cam_dir = (cam_pos - target).GetNormalized();
	cy::Vec3f up = { 0,1,0 };
	cy::Vec3f cam_r = up.Cross(cam_dir);
	cy::Vec3f cam_u = cam_dir.Cross(cam_r);
	cy::Matrix4f view = cy::Matrix4f::View(cam_pos, target, cam_u);
	cy::Matrix4f trans = cy::Matrix4f::Translation({ 0,0,-mid });
	cy::Matrix4f rtrans = cy::Matrix4f::Translation({ 0,0,mid });
	cy::Matrix3f xRot = cy::Matrix3f::RotationX(rotx);
	cy::Matrix3f zRot = cy::Matrix3f::RotationZ(rotz);
	cy::Matrix4f rot = rtrans*xRot*zRot*trans;
	cy::Matrix4f proj = cy::Matrix4f::Perspective(.7, float(width) / float(height), .1f, 1000.0f);

	return proj*view*rot;
}
cy::Matrix4f create_mv(float mid)
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
	cy::Matrix3f zRot = cy::Matrix3f::RotationZ(rotz);
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
	cy::Matrix3f zRot = cy::Matrix3f::RotationZ(rotz);
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
		dist -= (x - mouse_x) / 2;
	}
	else
	{
		if (x - mouse_x != 0)
			rotz += ((x - mouse_x)) * 3.14 / 180;
		if (y - mouse_y != 0)
			rotx += ((y - mouse_y) * 3.14) / 180;
	}
	cy::Matrix4f mvp = create_mvp(mid);
	prog["mvp"] = mvp;
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
	case 32://space
		a = a * 5;
		if (a > 50000)
		{
			a = 5;
  		}
		prog["alpha"] = a;
		break;
	case 25://f9
		prog.BuildFiles("shader.vert", "shader.frag");
		cy::Matrix4f mvp = create_mvp(mid);
		prog["mvp"] = mvp;
		break;
	case 112://p
		rotx = -1.5;
       		rotz = 0;
		dist = 100.0;
		cy::Matrix4f mvp2 = create_mvp(mid);
		prog["mvp"] = mvp2;
		break;
	}
	glutPostRedisplay();
	
}
void display_function()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	prog.Bind();
	glDrawArrays(GL_TRIANGLES, 0, buff_size);
	glDrawArrays(GL_TRIANGLES, 0, norm_buff_size);
	glutSwapBuffers();
}


