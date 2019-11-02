// CS370 - Fall 2018
// Assign03 - Limelight
//Daniel Palmieri T/TR 12:30-1:45
//Dr. Babcock - If you want to toggle lights, try using C (cube), O (octahedron), and P(sphere). If you want to change up the colors, try using M. To enter disco mode, use R.



#ifdef OSX
	#include <GLUT/glut.h>
#else
	#include <GL/glew.h>
	#include <GL/glut.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "materials.h"
#include "lighting.h"
#include "vectorops.h"
#include "globalVariables.h"

// Shader file utility functions
#include "shaderutils.h"

// Shader files
GLchar* lightVertexFile = "lightvert.vs";
GLchar* lightFragmentFile = "lightfrag.fs";
GLchar* defaultVertexFile = "basicvert.vs";
GLchar* defaultFragmentFile = "basicfrag.fs";

// Shader objects
GLuint lightShaderProg;
GLuint defaultShaderProg;
GLuint numLights_param;
GLint numLights = 4;

//recursive subdivision levels for stage
int div_level = 8;

#define DEG_TO_RAD 0.0175f
#define DEG_PER_SEC (360.0f/60.0f)
#define X 0
#define Y 1
#define Z 2
#define N 8
#define STAGE 1
#define CUBE 2
#define FULL_STAGE 3

// Global camera vectors
GLfloat eye[3] = {1.0f,1.0f,1.0f};
GLfloat at[3] = {0.0f,0.0f,0.0f};
GLfloat up[3] = {0.0f,1.0f,0.0f};

GLfloat eyePerspective[3] = { 0, 0, -5.0f };
GLfloat atPerspective[3] = { 0, 0, 0 };
GLfloat upPerspective[3] = { 0, 1, 0 };

// Global screen dimensions
GLint ww,hh;

void display();
void render_Scene(bool shadow);
void reshape(int w, int h);
void keyfunc(unsigned char key, int x, int y);
void idlefunc();
void colorcube();
void rquad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[]);
void div_quad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[], int n);
void create_lists();

int main(int argc, char* argv[])
{
	// Initialize glut
    glutInit(&argc, argv);

	// Initialize window
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Limelight");

#ifndef OSX
	// Initialize GLEW - MUST BE DONE AFTER CREATING GLUT WINDOW
	glewInit();
#endif

	// Define callbacks
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
	glutKeyboardFunc(keyfunc);
	glutIdleFunc(idlefunc);

	// Set background color to grey
	glClearColor(0.6f, 0.6f, 0.6f, 1.0f);

	// Enable depth test
    glEnable(GL_DEPTH_TEST);

	// Initialize shadow matrices
		// light 1 (lime cube)
		for (int i = 0; i < 16; i++)
			{
				M1_s[i] = 0.0f;
			}
		M1_s[0] = M1_s[5] = M1_s[10] = 1.0f;
		M1_s[7] = -1.0f / light1_pos[Y] ;

		// light 2 (red sphere)
		for (int i = 0; i < 16; i++)
		{
			M2_s[i] = 0.0f;
			}
		M2_s[0] = M2_s[5] = M2_s[10] = 1.0f;
		M2_s[7] = -1.0f / light2_pos[Y];


		// light 3 (blue octahedron)
		for (int i = 0; i < 16; i++)
		{
			M3_s[i] = 0.0f;
		}
		M3_s[0] = M3_s[5] = M3_s[10] = 1.0f;
		M3_s[7] = -1.0f / light3_pos[Y];

		// normalize initial direction
		normalize(light1_dir);
		normalize(light2_dir);
		normalize(light3_dir);

	// Load default shader programs
	defaultShaderProg = load_shaders(defaultVertexFile,defaultFragmentFile);
	// Load lighting shader programs
	lightShaderProg = load_shaders(lightVertexFile,lightFragmentFile);
	// Associate num_lights parameter
	numLights_param = glGetUniformLocation(lightShaderProg,"numLights");

	//Initialize quadric
	quadric = gluNewQuadric();
	gluQuadricDrawStyle(quadric, GLU_FILL);
	gluQuadricNormals(quadric, GLU_SMOOTH);

	//create lists
	create_lists();

	// Start graphics loop
	glutMainLoop();
}

// Display callback
void display()
{
	// Reset background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Set projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Adjust viewing volume (orthographic)
	GLfloat xratio = 1.0f;
	GLfloat yratio = 1.0f;
	// If taller than wide adjust y
	if (ww <= hh)
	{
		yratio = (GLfloat)hh / (GLfloat)ww;
	}
	// If wider than tall adjust x
	else if (hh <= ww)
	{
		xratio = (GLfloat)ww / (GLfloat)hh;
	}
	glOrtho(-10.0f*xratio, 10.0f*xratio, -10.0f*yratio, 10.0f*yratio, -10.0f, 10.0f);

	// Set modelview matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye[X], eye[Y], eye[Z], at[X], at[Y], at[Z], up[X], up[Y], up[Z]);

	// Render scene without shadows
	render_Scene(false);

	// Render scene with shadows
	render_Scene(true);

	// Flush buffer
	glFlush();

	// Swap buffers
	glutSwapBuffers();
}

void render_Scene(bool shadow)
{
	
	// Set lights on stage
	set_PointLight(GL_LIGHT0, &white_light, light0_pos);

	//disco mode
	if (disco_mode) {
		set_SpotLight(GL_LIGHT1, &red_light, light1_pos, light1_dir, light1_cutoff, light1_exp);
		set_SpotLight(GL_LIGHT2, &blue_light, light2_pos, light2_dir, light2_cutoff, light2_exp);
		set_SpotLight(GL_LIGHT3, &lime_light, light3_pos, light3_dir, light3_cutoff, light3_exp);
	}
	else {
		set_SpotLight(GL_LIGHT1, &lime_light, light1_pos, light1_dir, light1_cutoff, light1_exp);
		set_SpotLight(GL_LIGHT2, &red_light, light2_pos, light2_dir, light2_cutoff, light2_exp);
		set_SpotLight(GL_LIGHT3, &blue_light, light3_pos, light3_dir, light3_cutoff, light3_exp);
	}

	//color swapping
	if (color_swap) {
		set_SpotLight(GL_LIGHT1, &red_light, light1_pos, light1_dir, light1_cutoff, light1_exp);
		set_SpotLight(GL_LIGHT2, &blue_light, light2_pos, light2_dir, light2_cutoff, light2_exp);
		set_SpotLight(GL_LIGHT3, &lime_light, light3_pos, light3_dir, light3_cutoff, light3_exp);
	}
	else {
		set_SpotLight(GL_LIGHT1, &lime_light, light1_pos, light1_dir, light1_cutoff, light1_exp);
		set_SpotLight(GL_LIGHT2, &red_light, light2_pos, light2_dir, light2_cutoff, light2_exp);
		set_SpotLight(GL_LIGHT3, &blue_light, light3_pos, light3_dir, light3_cutoff, light3_exp);
	}
	

	////////////////////////////////// Begin Stage
	if (!shadow) // Stage does not have a shadow
	{ 
		// Stage
		glUseProgram(lightShaderProg);
		glUniform1i(numLights_param, numLights);

		glPushMatrix();
		glTranslatef(0.5f, 0.0f, -1.0f);
		glCallList(FULL_STAGE);
		glPopMatrix();

	} 
	////////////////////////////////////////END stage
	

	////////////////////////////////////// Begin Cube
	glPushMatrix();
		if (!shadow || !light1_disp) //normal OR light is off
		{
			glUseProgram(lightShaderProg);
			glUniform1i(numLights_param, numLights);
			set_material(GL_FRONT_AND_BACK, &brass);
		}
		else // else there is a shadow, set matrix and use default shader
		{
			//Set shadow matrix
			glTranslatef(light1_pos[0], light1_pos[1], light1_pos[2]);
			glMultMatrixf(M1_s);
			glTranslatef(-light1_pos[0], -light1_pos[1], -light1_pos[2]);

			// Use default shader
			glUseProgram(defaultShaderProg);

			//Set shadow color
			glColor3fv(shadow_color);
		}
			//draw cube
			glTranslatef(cube_pos[X], cube_pos[Y], cube_pos[Z]);
			glScalef(0.5, 0.5, 0.5);
			glCallList(STAGE);
	glPopMatrix();
	////////////////////////////////////// //END Cube
							

	////////////////////////////////////// Begin Sphere
	glPushMatrix();
		if (!shadow || !light2_disp) // normal OR light is off
		{
			glUseProgram(lightShaderProg);
			glUniform1i(numLights_param, numLights);
			if (color_swap) {
				set_material(GL_FRONT_AND_BACK, &blue);
			}
			else {
				set_material(GL_FRONT_AND_BACK, &ruby);
			}
		}
		else  //else there is a shadow, set matrixand use default shader
		{
			// Set shadow matrix
			glTranslatef(light2_pos[0], light2_pos[1], light2_pos[2]);
			glMultMatrixf(M2_s);
			glTranslatef(-light2_pos[0], -light2_pos[1], -light2_pos[2]);

			// Use default shader
			glUseProgram(defaultShaderProg);

			// Set shadow color
			glColor3fv(shadow_color);
		}
		// draw sphere
			glTranslatef(sphere_pos[X], sphere_pos[Y], sphere_pos[Z]);
			gluSphere(quadric,sphere_radius, sphere_slices, sphere_stacks);
	glPopMatrix();
	///////////////////////////////////////////END sphere


	/////////////////////////////////////////// Begin Octahedron
	glPushMatrix();
		if (!shadow || !light3_disp) //normal OR light is off
		{
			glUseProgram(lightShaderProg);
			glUniform1i(numLights_param, numLights);
			set_material(GL_FRONT_AND_BACK, &blue);
		}
		else // else there is a shadow, set matrix and use default shader
		{
			// Set shadow matrix
			glTranslatef(light3_pos[0], light3_pos[1], light3_pos[2]);
			glMultMatrixf(M3_s);
			glTranslatef(-light3_pos[0], -light3_pos[1], -light3_pos[2]);

			// Use default shader
			glUseProgram(defaultShaderProg);

			//Set shadow color
			glColor3fv(shadow_color);
		}
		//draw octahedron
			glTranslatef(octa_x_shift, octa_y_shift, octa_z_shift);
			glRotatef(octa_theta, 0, 1, 0);
			glScalef(1.25f, 1.25f, 1.25f);
			glutSolidOctahedron();
	
	glPopMatrix();
	//////////////////////////////////////////// END Octahedron

	
}

// Reshape callback
void reshape(int w, int h)
{
	// Set new screen extents
	glViewport(0, 0, w, h);

	// Store new extents
	ww = w;
	hh = h;
}

// Keyboard callback
void keyfunc(unsigned char key, int x, int y)
{
	// Keypress functionality

	//‘WASD’ pivots cube light source in the x and z directions
	if (key == 'w' || key == 'W')
	{
		light1_pos[Z] += LIGHT_STEP;
		light1_dir[X] = (cube_x_shift) - light1_pos[X];
		light1_dir[Y] = (stage_scaleY) - light1_pos[Y];
		light1_dir[Z] = (cube_z_shift) - light1_pos[Z];
		normalize(light1_dir);  //normalize result

	}
	else if (key == 's' || key == 'S')
	{
		light1_pos[Z] -= LIGHT_STEP;
		light1_dir[X] = (cube_x_shift)-light1_pos[X];
		light1_dir[Y] = (stage_scaleY)-light1_pos[Y];
		light1_dir[Z] = (cube_z_shift)-light1_pos[Z];
		normalize(light1_dir); //normalize result
	}

	if (key == 'a' || key == 'A')
	{
		light1_pos[X] += LIGHT_STEP;
		light1_dir[X] = (cube_x_shift)-light1_pos[X];
		light1_dir[Y] = (stage_scaleY)-light1_pos[Y];
		light1_dir[Z] = (cube_z_shift)-light1_pos[Z];
		normalize(light1_dir); //normalize result
	}
	else if (key == 'd' || key == 'D')
	{
		light1_pos[X] -= LIGHT_STEP;
		light1_dir[X] = (cube_x_shift)-light1_pos[X];
		light1_dir[Y] = (stage_scaleY)-light1_pos[Y];
		light1_dir[Z] = (cube_z_shift)-light1_pos[Z];
		normalize(light1_dir); //normalize result
	}
	
	//‘TFGH pivots sphere light source in the x and z directions
	if (key == 't' || key == 'T')
	{
		light2_pos[Z] += LIGHT_STEP;
		light2_dir[X] = (sphere_x_shift)-light2_pos[X];
		light2_dir[Y] = (stage_scaleY)-light2_pos[Y];
		light2_dir[Z] = (sphere_z_shift)-light2_pos[Z];
		normalize(light2_dir); //normalize result
		
	}
	else if (key == 'g' || key == 'G')
	{
		light2_pos[Z] -= LIGHT_STEP;
		light2_dir[X] = (sphere_x_shift)-light2_pos[X];
		light2_dir[Y] = (stage_scaleY)-light2_pos[Y];
		light2_dir[Z] = (sphere_z_shift)-light2_pos[Z];
		normalize(light2_dir); //normalize result
	}
	
	if (key == 'f' || key == 'F')
	{
		light2_pos[0] += LIGHT_STEP;
		light2_dir[X] = (sphere_x_shift)-light2_pos[X];
		light2_dir[Y] = (stage_scaleY)-light2_pos[Y];
		light2_dir[Z] = (sphere_z_shift)-light2_pos[Z];
		normalize(light2_dir); //normalize result
	}
	else if (key == 'h' || key == 'H')
	{
		light2_pos[X] -= LIGHT_STEP;
		light2_dir[X] = (sphere_x_shift)-light2_pos[X];
		light2_dir[Y] = (stage_scaleY)-light2_pos[Y];
		light2_dir[Z] = (sphere_z_shift)-light2_pos[Z];
		normalize(light2_dir); //normalize result
	}

	//‘IJKL’ pivots octahedron light source in the x and z directions
	if (key == 'i' || key == 'I')
	{
		light3_pos[Z] += LIGHT_STEP;
		light3_dir[X] = (octa_x_shift)-light3_pos[X];
		light3_dir[Y] = (stage_scaleY)-light3_pos[Y];
		light3_dir[Z] = (octa_z_shift)-light3_pos[Z];
		normalize(light3_dir); //normalize result
	}
	else if (key == 'k' || key == 'K')
	{
		light3_pos[Z] -= LIGHT_STEP;
		light3_dir[X] = (octa_x_shift)-light3_pos[X];
		light3_dir[Y] = (stage_scaleY)-light3_pos[Y];
		light3_dir[Z] = (octa_z_shift)-light3_pos[Z];
		normalize(light3_dir); //normalize result
	}
	if (key == 'j' || key == 'J')
	{
		light3_pos[X] += LIGHT_STEP;
		light3_dir[X] = (octa_x_shift)-light3_pos[X];
		light3_dir[Y] = (stage_scaleY)-light3_pos[Y];
		light3_dir[Z] = (octa_z_shift)-light3_pos[Z];
		normalize(light3_dir); //normalize result
	}
	else if (key == 'l' || key == 'L')
	{
		light3_pos[X] -= LIGHT_STEP;
		light3_dir[X] = (octa_x_shift)-light3_pos[X];
		light3_dir[Y] = (stage_scaleY)-light3_pos[Y];
		light3_dir[Z] = (octa_z_shift)-light3_pos[Z];
		normalize(light3_dir); //normalize result
	}
	
	//‘N’ should toggle the octahedron spinning / stopped
	if (key == 'n' || key == 'N') 
	{
		octa_spin = !octa_spin;
	}

	//‘B’ should toggle the sphere bouncing up / down
	if (key == 'b' || key == 'B')
	{
		bounce_sphere = !bounce_sphere;
	}
	
	//‘V’ should toggle the cube sliding forward / backwards
	if (key == 'V' || key == 'v') 
	{
		cube_move = !cube_move;
	}

	// C, O, and P toggle lights (C for cube, O for octahedron, and P for sphere
	if (key == 'C' || key == 'c') {
		light1_dir[Y] *= -1;
		light1_disp = !light1_disp;
	}
	if (key == 'O' || key == 'o') {
		light3_dir[Y] *= -1;
		light3_disp = !light3_disp;
	}
	if (key == 'P' || key == 'p') {
		light2_dir[Y] *= -1;
		light2_disp = !light2_disp;
	}
	if (key == 'm' || key == 'M') {
		color_swap = !color_swap;
	}

	if (key == 'r' || key == 'R') {
		disco_mode = !disco_mode;
	}

	// Esc to quit
	if (key == 27)
	{
		exit(0);
	}
	glutPostRedisplay();
}

void colorcube()
{
	// Top face
	div_quad(cube[4], cube[7], cube[6], cube[5], div_level);

	// Bottom face
	rquad(cube[0], cube[1], cube[2], cube[3]);

	// Left face
	rquad(cube[0], cube[3], cube[7], cube[4]);

	// Right face
	rquad(cube[1], cube[5], cube[6], cube[2]);

	// Front face
	rquad(cube[2], cube[6], cube[7], cube[3]);

	// Back face
	rquad(cube[0], cube[4], cube[5], cube[1]);
}

// Routine to perform recursive subdivision
void div_quad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[], int n)
{
	GLfloat v1_prime[3], v2_prime[3], v3_prime[3], v4_prime[3], v5_prime[3];

	// Recurse until n = 0
	if (n > 0)
	{
		// Compute midpoints
		for (int i = 0; i < 3; i++)
		{
			v1_prime[i] = (v4[i] + v1[i]) / 2.0f;
			v2_prime[i] = (v1[i] + v2[i]) / 2.0f;
			v3_prime[i] = (v2[i] + v3[i]) / 2.0f;
			v4_prime[i] = (v3[i] + v4[i]) / 2.0f;
			v5_prime[i] = (v1[i] + v2[i] + v3[i] + v4[i]) / 4.0f;
		}

		// Subdivide polygon
		div_quad(v1, v2_prime, v5_prime, v1_prime, n - 1);
		div_quad(v2_prime, v2, v3_prime, v5_prime, n - 1);
		div_quad(v1_prime, v5_prime, v4_prime, v4, n - 1);
		div_quad(v5_prime, v3_prime, v3, v4_prime, n - 1);
	}
	else
	{
		// Otherwise render quad
		rquad(v1, v2, v3, v4);
	}
}

// Routine to draw quadrilateral face
void rquad(GLfloat v1[], GLfloat v2[], GLfloat v3[], GLfloat v4[])
{
	GLfloat normal[3];
	// Compute normal via cross product
	cross(v1, v2, v4, normal);
	normalize(normal);

	// Set normal
	glNormal3fv(normal);

	// Draw face 
	glBegin(GL_POLYGON);
	glVertex3fv(v1);
	glVertex3fv(v2);
	glVertex3fv(v3);
	glVertex3fv(v4);
	glEnd();
}

// Idle callback
void idlefunc()
{
	// Time-based Animations
	time = glutGet(GLUT_ELAPSED_TIME);
	//Update if past desired interval
	if (time - lasttime > 1000.0f / fps)
	{
		// Compute  changes
		octa_dtheta = 10.0f * rpm * (time - lasttime) / 1000.0f;
		sphere_bounce_dy =  SPHERE_STEP * (time - lasttime)/50.0f;
		cube_shift_dz = 4.0f * rpm * CUBE_STEP *(time - lasttime) / 1000.0f;

		if (octa_spin)
		{ //update current position, make sure it does not exceed [0,360]
			octa_theta += octa_dtheta;
			if (octa_theta >= 360) {
				octa_theta = octa_theta / 360;
			}
		}

		if (bounce_sphere)
		{//update current position, make sure it does not exceed bounds
			sphere_pos[Y] += (sphere_bounce_dy* sphere_dir);
			if ((sphere_pos[Y] > SPHERE_MAX) || (sphere_pos[Y] < SPHERE_MIN))
			{
				sphere_dir *= -1;
			}
		}

		if (cube_move)
		{//update current position, make sure it does not exceed bounds
			cube_pos[Z] += cube_shift_dz *cube_dir;
			if ((cube_pos[Z] > CUBE_MAX) || (cube_pos[Z] < CUBE_MIN))
			{
				cube_dir *= -1;
			}
		}

		//disco mode
		if (disco_mode) {
			if (time % 20  > 15) { // swap lights every few seconds
				color_swap = !color_swap;
			}
		}
		//update lasttime (reset timer)
		lasttime = time;
		glutPostRedisplay();
	}
}

void create_lists()
{
	// Create colorcube display list
	glNewList(STAGE, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);
	colorcube();
	glPopAttrib();
	glEndList();


	glNewList(FULL_STAGE, GL_COMPILE);
	glPushAttrib(GL_CURRENT_BIT);
	glPushMatrix();
	// base cube
	set_material(GL_FRONT_AND_BACK, &brass);
	glTranslatef(0.0f, -0.5f, -0.5f);
	glScalef(stage_scaleX, stage_scaleY, stage_scaleZ);
	glCallList(STAGE);
	glPopMatrix();

	//curved edge of stage
	glPushMatrix();
	set_material(GL_FRONT_AND_BACK, &brass);
	glTranslatef(curve_x_shift, -0.15035, curve_z_shift);
	glRotatef(90, 1, 0, 0);
	glScalef(curve_x_scale, curve_y_scale, curve_z_scale);
	gluCylinder(quadric, curve_baseRad, curve_topRad, curve_height, curve_slices, curve_stacks);
	glPopMatrix();

	glPushMatrix();
	set_material(GL_FRONT_AND_BACK, &brass);
	glTranslatef(curve_x_shift, -0.15015, curve_z_shift);
	glRotatef(-90, 1, 0, 0);
	glScalef(curve_x_scale, curve_y_scale, curve_z_scale);
	gluDisk(quadric, 0, curve_topRad, curve_slices, curve_stacks);
	glPopMatrix();
	glPopAttrib();
	glEndList();
											

}