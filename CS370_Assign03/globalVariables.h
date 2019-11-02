
//Object global variables
GLUquadricObj* quadric;

// symbolic constants
#define DEG_TO_RAD 0.0175f
#define DEG_PER_SEC (360.0f/60.0f)
#define X 0
#define Y 1
#define Z 2
#define N 8
#define STAGE 1
#define CUBE 2
#define FULL_STAGE 3

#define LIGHT_STEP 0.1f

#define SPHERE_STEP 0.05f
#define SPHERE_MAX 2.0f
#define SPHERE_MIN 0.8f

#define CUBE_STEP 0.05f
#define CUBE_MIN -1.25f // TOWARDS BACK
#define CUBE_MAX 1.5f //TOWARDS FRONT

// Global lights variables
GLenum lights[4] = { GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3 };
bool light1_disp = true;
bool light2_disp = true;
bool light3_disp = true;
bool color_swap = false;
bool disco_mode = false;


GLfloat light_z_dist = 5;
GLfloat light_y_dist = 10;

// cube variables
GLfloat cube_x_shift = -4.75f;
GLfloat cube_y_shift = 0.5f;
GLfloat cube_z_shift = 0.0;
GLfloat cube_pos[] = { cube_x_shift, cube_y_shift, cube_z_shift };
GLfloat cube_shift_dz = 0;
int cube_dir = 1;
bool cube_move = false;

// sphere variables
GLfloat sphere_x_shift = 0.0f;
GLfloat sphere_y_shift = SPHERE_MIN;
GLfloat sphere_z_shift = 0.0;
GLfloat sphere_pos[] = { sphere_x_shift, sphere_y_shift, sphere_z_shift };
GLfloat sphere_radius = 1;
GLfloat sphere_slices = 50;
GLfloat sphere_stacks = 50;
GLfloat sphere_bounce_dy = 0;
int sphere_dir = 1;
bool bounce_sphere = false;

// octahedron variabless
GLfloat octa_x_shift = 5.0f;
GLfloat octa_y_shift = 1.25f;
GLfloat octa_z_shift = 0.0;
GLfloat octa_theta = 0.0f;
GLfloat octa_dtheta = 0.0f;
bool octa_spin = false;

// stage variables
GLfloat stage_scaleX = 8.399;
GLfloat stage_scaleY = 0.35;
GLfloat stage_scaleZ = 3.25;

GLfloat curve_baseRad = 4;
GLfloat curve_topRad = curve_baseRad;
GLfloat curve_height = 1.0f;
GLfloat curve_stacks = 200;
GLfloat curve_slices = 200;

GLfloat curve_x_shift = 0.00f;
GLfloat curve_y_shift = 0.0f;
GLfloat curve_z_shift = 2.10;

GLfloat curve_x_scale = 2.1090;
GLfloat curve_y_scale = 1.199;
GLfloat curve_z_scale = 0.70;




// Global animation variables
GLint time = 0;
GLint lasttime = 0;
GLint fps = 60;
GLfloat rpm = 10.0f;

// cube array
GLfloat cube[][3] = { {-1.0f,-1.0f,-1.0f},{1.0f,-1.0f,-1.0f},{1.0f,-1.0f,1.0f},
					 {-1.0f,-1.0f,1.0f},{-1.0f,1.0f,-1.0f},{1.0f,1.0f,-1.0f},
					 {1.0f,1.0f,1.0f},{-1.0f,1.0f,1.0f} };
// Light0 (point) Parameters
GLfloat light0_pos[] = { 0.0f, 5.0f, 1.0f, 1.0f }; // white light

//light1 (green spotlight) Parameters 
GLfloat light1_pos[] = { cube_x_shift, light_y_dist, cube_z_shift + light_z_dist, 1.0f };
GLfloat light1_dir[] = { (cube_x_shift - light1_pos[X]), (cube_y_shift - light1_pos[Y]) , (cube_z_shift - light1_pos[Z]) };
GLfloat light1_cutoff = 10;
GLfloat light1_exp = 3;

// Light2 (red spotlight) parameters
GLfloat light2_pos[] = { sphere_x_shift, light_y_dist, sphere_z_shift + light_z_dist, 1.0f };
GLfloat light2_dir[] = { (sphere_x_shift - light2_pos[X]), (sphere_y_shift - light2_pos[Y]),  (sphere_z_shift-light2_pos[Z]) };
GLfloat light2_cutoff = 10;
GLfloat light2_exp = 3;

// light 3 (blue spotlight) parameters
GLfloat light3_pos[] = { octa_x_shift, light_y_dist, octa_z_shift + light_z_dist, 1.0f };
GLfloat light3_dir[] = { (octa_x_shift - light3_pos[X]), (octa_y_shift - light3_pos[Y]), (octa_z_shift - light3_pos[Z]) };
GLfloat light3_cutoff = 10;
GLfloat light3_exp = 3;

// Shadow matrices
GLfloat M1_s[16];
GLfloat M2_s[16];
GLfloat M3_s[16];
GLfloat shadow_color[] = { 0.2f,0.2f,0.2f };