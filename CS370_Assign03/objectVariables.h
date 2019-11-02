
GLUquadricObj* quadric;

#define SPHERE_STEP 0.05f
#define SPHERE_MAX 2.5f
#define SPHERE_MIN (1.0f)

#define CUBE_STEP 0.05f
#define CUBE_MIN -1.25f // TOWARDS BACK
#define CUBE_MAX 1.5f //TOWARDS FRONT

#define LIGHT_STEP 0.1f
#define X 0
#define Y 1
#define Z 2

// Global variables
GLenum lights[4] = { GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3 };

GLfloat light_z_dist = 5;
GLfloat light_y_dist = 10;

// Light0 (point) Parameters
GLfloat light0_pos[] = { 0.0f, 5.0f, 1.0f, 1.0f }; // white light

//light1 (green spotlight) Parameters 
GLfloat light1_pos[] = { cube_x_shift, light_y_dist, cube_z_shift + light_z_dist, 1.0f }; //Direction = position of cube - position light (then normalize result). Move lights until it looks right.
GLfloat light1_dir[] = { (cube_x_shift - light1_pos[X]), (cube_y_shift - light1_pos[Y]) , (cube_z_shift - light1_pos[Z]) };
GLfloat light1_cutoff = 10;
GLfloat light1_exp = 3;

// Light2 (red spotlight) parameters
GLfloat light2_pos[] = { sphere_x_shift, light_y_dist, sphere_z_shift - light_z_dist, 1.0f };
GLfloat light2_dir[] = { (sphere_x_shift - light2_pos[X]), (sphere_y_shift - light2_pos[Y]), (0 - light2_pos[Z]) };
GLfloat light2_cutoff = 10;
GLfloat light2_exp = 3;

// light 3 (blue spotlight) parameters
GLfloat light3_pos[] = { octa_x_shift, light_y_dist, octa_z_shift + light_z_dist, 1.0f };
GLfloat light3_dir[] = { (octa_x_shift - light3_pos[X]),(octa_y_shift - light3_pos[Y]), (octa_z_shift - light3_pos[Z]) };
GLfloat light3_cutoff = 10;
GLfloat light3_exp = 3;

// Shadow matrices
GLfloat M1_s[16];
GLfloat M2_s[16];
GLfloat M3_s[16];
GLfloat shadow_color[] = { 0.2f,0.2f,0.2f };


GLfloat cube_x_shift = -5.0f;
GLfloat cube_y_shift = 0.5f;
GLfloat cube_z_shift = 0.0;
GLfloat cube_pos[] = { cube_x_shift, cube_y_shift, cube_z_shift };
int cube_dir = 1;
bool cube_move = false;


GLfloat sphere_x_shift = 0.0f;
GLfloat sphere_y_shift = SPHERE_MIN;
GLfloat sphere_z_shift = 0.0;
GLfloat sphere_pos[] = { sphere_x_shift, sphere_y_shift, sphere_z_shift };
GLfloat sphere_radius = 1;
GLfloat sphere_slices = 50;
GLfloat sphere_stacks = 50;
int sphere_dir = 1;
bool bounce_sphere = false;

GLfloat octa_x_shift = 5.0f;
GLfloat octa_y_shift = 1.25f;
GLfloat octa_z_shift = 0.0;
GLfloat octa_theta = 0.0f;
GLfloat octa_dtheta = 3.0f;
bool octa_spin = false;

GLfloat stage_scaleX = 8;
GLfloat stage_scaleY = 0.4;
GLfloat stage_scaleZ = 4;


// cube array
GLfloat cube[][3] = { {-1.0f,-1.0f,-1.0f},{1.0f,-1.0f,-1.0f},{1.0f,-1.0f,1.0f},
					 {-1.0f,-1.0f,1.0f},{-1.0f,1.0f,-1.0f},{1.0f,1.0f,-1.0f},
					 {1.0f,1.0f,1.0f},{-1.0f,1.0f,1.0f} };