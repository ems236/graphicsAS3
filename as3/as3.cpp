//ems236 Assignment 3

#include "Coordinate.h";
#include "Matrix.h";
#include <stdio.h>
#include <stdlib.h>
#include <GL\glut.h>
#include <math.h>
#include <vector>

#define ON 1
#define OFF 0
#define PI 3.14159265

const double rotation_base_angle = 0.3;
const double translation_base = 0.5;
const double scale_base = 0.2;

// Global variables
int window_width, window_height;    // Window dimensions

//Current scene settings
int PERSPECTIVE = OFF;
int SHOW_AXES = ON;
int NEXT_LOOKAT_IS_WORLD = ON;
int LEFT_MOUSE_DOWN = OFF;
int RIGHT_MOUSE_DOWN = OFF;
int MIDDLE_MOUSE_DOWN = OFF;
int MOUSE_LAST_X = NULL;
int MOUSE_LAST_Y = NULL;
int OBJECT_HAS_LOADED = OFF;

//current camera settings
Coordinate camera_position;
Coordinate look_at;
Coordinate up_vector;

//Local transforms
Matrix model_rotation;
double scale = 1;

//world transforms
Matrix model_translation;
Matrix world_rotation;

//Both of these apply scaling
Matrix model_to_view;
Matrix world_to_view;

// Vertex and Face data structure used in the mesh reader
// Feel free to change them
typedef struct _point {
  float x,y,z;
} point;

typedef struct _faceStruct {
  int v1,v2,v3;
  int n1,n2,n3;
} faceStruct;

int verts, faces, norms;    // Number of vertices, faces and normals in the system
point *vertList, *normList; // Vertex and Normal Lists
faceStruct *faceList;	    // Face List


// The mesh reader itself
// It can read *very* simple obj files
void meshReader (char *filename,int sign)
{
	float x,y,z,len;
	int i;
	char letter;
	point v1,v2,crossP;
	int ix,iy,iz;
	int *normCount;
	FILE *fp;

	fp = fopen(filename, "r");
	if (fp == NULL) { 
		printf("Cannot open %s\n!", filename);
		exit(0);
	}

	// Count the number of vertices and faces
	while(!feof(fp))
	{
		fscanf(fp,"%c %f %f %f\n",&letter,&x,&y,&z);
		if (letter == 'v')
		{
			verts++;
		}
		else
		{
			faces++;
		}
	}

	fclose(fp);

	printf("verts : %d\n", verts);
	printf("faces : %d\n", faces);

	// Dynamic allocation of vertex and face lists
	faceList = (faceStruct *)malloc(sizeof(faceStruct)*faces);
	vertList = (point *)malloc(sizeof(point)*verts);
	normList = (point *)malloc(sizeof(point)*verts);

	fp = fopen(filename, "r");

	// Read the veritces
	for(i = 0;i < verts;i++)
	{
		fscanf(fp,"%c %f %f %f\n",&letter,&x,&y,&z);
		vertList[i].x = x;
		vertList[i].y = y;
		vertList[i].z = z;
	}

	// Read the faces
	for(i = 0;i < faces;i++)
	{
		fscanf(fp,"%c %d %d %d\n",&letter,&ix,&iy,&iz);
		faceList[i].v1 = ix - 1;
		faceList[i].v2 = iy - 1;
		faceList[i].v3 = iz - 1;
	}
	fclose(fp);


	// The part below calculates the normals of each vertex
	normCount = (int *)malloc(sizeof(int)*verts);
	for (i = 0;i < verts;i++)
	{
		normList[i].x = normList[i].y = normList[i].z = 0.0;
		normCount[i] = 0;
	}

	for(i = 0;i < faces;i++)
	{
		v1.x = vertList[faceList[i].v2].x - vertList[faceList[i].v1].x;
		v1.y = vertList[faceList[i].v2].y - vertList[faceList[i].v1].y;
		v1.z = vertList[faceList[i].v2].z - vertList[faceList[i].v1].z;
		v2.x = vertList[faceList[i].v3].x - vertList[faceList[i].v2].x;
		v2.y = vertList[faceList[i].v3].y - vertList[faceList[i].v2].y;
		v2.z = vertList[faceList[i].v3].z - vertList[faceList[i].v2].z;

		crossP.x = v1.y*v2.z - v1.z*v2.y;
		crossP.y = v1.z*v2.x - v1.x*v2.z;
		crossP.z = v1.x*v2.y - v1.y*v2.x;

		len = sqrt(crossP.x*crossP.x + crossP.y*crossP.y + crossP.z*crossP.z);

		crossP.x = -crossP.x/len;
		crossP.y = -crossP.y/len;
		crossP.z = -crossP.z/len;

		normList[faceList[i].v1].x = normList[faceList[i].v1].x + crossP.x;
		normList[faceList[i].v1].y = normList[faceList[i].v1].y + crossP.y;
		normList[faceList[i].v1].z = normList[faceList[i].v1].z + crossP.z;
		normList[faceList[i].v2].x = normList[faceList[i].v2].x + crossP.x;
		normList[faceList[i].v2].y = normList[faceList[i].v2].y + crossP.y;
		normList[faceList[i].v2].z = normList[faceList[i].v2].z + crossP.z;
		normList[faceList[i].v3].x = normList[faceList[i].v3].x + crossP.x;
		normList[faceList[i].v3].y = normList[faceList[i].v3].y + crossP.y;
		normList[faceList[i].v3].z = normList[faceList[i].v3].z + crossP.z;
		normCount[faceList[i].v1]++;
		normCount[faceList[i].v2]++;
		normCount[faceList[i].v3]++;
	}

	for (i = 0;i < verts;i++)
	{
		normList[i].x = (float)sign*normList[i].x / (float)normCount[i];
		normList[i].y = (float)sign*normList[i].y / (float)normCount[i];
		normList[i].z = (float)sign*normList[i].z / (float)normCount[i];
	}
}




void draw_vertex(Coordinate& v)
{
	glVertex3f(v.x(), v.y(), v.z());
}

//Gives z axis. Camera looks down negative z axis
Coordinate current_viewing_vector()
{
	return Coordinate::vector3(
		camera_position.x() - look_at.x()
		, camera_position.y() - look_at.y()
		, camera_position.z() - look_at.z()
	).normalized();
}

Matrix current_viewing_transform()
{
	Coordinate n = current_viewing_vector();

	//& is cross product
	Coordinate u = (up_vector & n).normalized();
	Coordinate v = (n & u).normalized();

	//get the inverse
	Matrix Mvw = *new Matrix();
	Mvw.add(Coordinate::vector3(u.x(), v.x(), n.x()));
	Mvw.add(Coordinate::vector3(u.y(), v.y(), n.y()));
	Mvw.add(Coordinate::vector3(u.z(), v.z(), n.z()));

	//a temp to make multiplication work
	Mvw.add(Coordinate::point3(0, 0, 0));

	//multiply by -1 hardcoded because I don't want to make that operator
	Coordinate camera_to_world_translation = Coordinate::vector3(-1 * camera_position.x(), -1 * camera_position.y(), -1 * camera_position.z());
	Coordinate translation_inverse = Mvw * camera_to_world_translation;
	translation_inverse.homogenize(true);

	Mvw.data.pop_back();
	Mvw.add(translation_inverse);

	return Mvw;
}

void update_state()
{
	Matrix scale_matrix = Matrix::scale(scale);
	Matrix current_viewing = current_viewing_transform();
	model_to_view = current_viewing * world_rotation * model_translation * model_rotation * scale_matrix;
	world_to_view = current_viewing * scale_matrix;

	glutPostRedisplay();
}

Coordinate transform_point(Coordinate point)
{
	return model_to_view * point;
}

Coordinate transform_point(point* point)
{
	return transform_point(Coordinate::point3(point->x, point->y, point->z));
}

Coordinate transform_world_point(Coordinate point)
{
	return world_to_view * point;
}

Coordinate transform_world_point(point* point)
{
	return transform_world_point(Coordinate::point3(point->x, point->y, point->z));
}

void draw_axes(Coordinate origin, Coordinate x, Coordinate y, Coordinate z)
{
	glBegin(GL_LINES);
	//x
	glColor3f(1, 0, 0);
	glVertex3f(origin.x(), origin.y(), origin.z());
	glVertex3f(x.x(), x.y(), x.z());

	//y
	glColor3f(0, 1, 0);
	glVertex3f(origin.x(), origin.y(), origin.z());
	glVertex3f(y.x(), y.y(), y.z());

	//z
	glColor3f(0, 0, 1);
	glVertex3f(origin.x(), origin.y(), origin.z());
	glVertex3f(z.x(), z.y(), z.z());
	glEnd();
}

void draw_object()
{
	if (!OBJECT_HAS_LOADED)
	{
		meshReader("teapot.obj", 1);
		OBJECT_HAS_LOADED = ON;
	}

	glColor3f(1, 1, 1);

	glBegin(GL_TRIANGLES);
		for(int i = 0; i < faces; i++)
		{
			draw_vertex(transform_point(&vertList[faceList[i].v1]));
			draw_vertex(transform_point(&vertList[faceList[i].v2]));
			draw_vertex(transform_point(&vertList[faceList[i].v3]));
		}
	glEnd();
}

// The display function. It is called whenever the window needs
// redrawing (ie: overlapping window moves, resize, maximize)
// You should redraw your polygons here
void display(void)
{
    // Clear the background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	char* projection_type_description;

	if (PERSPECTIVE) {
		// Perpective Projection 
		gluPerspective(60, (GLdouble)window_width / window_height, 0.01, 10000);
		glutSetWindowTitle("ems236 assignment 3 Perspective Mode");
	}
	else {
		// Orthogonal Projection 
		glOrtho(-2.5, 2.5, -2.5, 2.5, -10000, 10000);
		glutSetWindowTitle("ems236 assignment 3 Orthographic Mode");
	}
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (SHOW_AXES)
	{
		Coordinate zero = Coordinate::point3(0, 0, 0);
		Coordinate x = Coordinate::point3(1, 0, 0);
		Coordinate y = Coordinate::point3(0, 1, 0);
		Coordinate z = Coordinate::point3(0, 0, 1);

		draw_axes(transform_point(zero), transform_point(x), transform_point(y), transform_point(z));
		draw_axes(transform_world_point(zero), transform_world_point(x), transform_world_point(y), transform_world_point(z));
	}

	draw_object();

    // (Note that the origin is lower left corner)
    // (Note also that the window spans (0,1) )
    // Finish drawing, update the frame buffer, and swap buffers
    glutSwapBuffers();
}


// This function is called whenever the window is resized. 
// Parameters are the new dimentions of the window
void resize(int x,int y)
{
    glViewport(0,0,x,y);
    window_width = x;
    window_height = y;
    if (PERSPECTIVE) {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(60,(GLdouble) window_width/window_height,0.01, 10000);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
    }
    printf("Resized to %d %d\n",x,y);
}

int* getFlagByMouseType(int mouseType)
{
	const int LEFT = 0;
	const int MIDDLE = 1;
	const int RIGHT = 2;

	if (mouseType == LEFT)
	{
		return &LEFT_MOUSE_DOWN;
	}
	if (mouseType == MIDDLE)
	{
		return &LEFT_MOUSE_DOWN;
	}
	if (mouseType == RIGHT)
	{
		return &RIGHT_MOUSE_DOWN;
	}
}

void set_mouse_pos(int x, int y)
{
	MOUSE_LAST_X = x;
	MOUSE_LAST_Y = y;
}

// This function is called whenever the mouse is pressed or released
// button is a number 0 to 2 designating the button
// state is 1 for release 0 for press event
// x and y are the location of the mouse (in window-relative coordinates)
void mouseButton(int button,int state,int x,int y)
{
	const int DOWN = 0;
	const int UP = 1;
	
	set_mouse_pos(x, y);

	int is_mouse_down = ON;
	if (state == UP)
	{
		is_mouse_down = OFF;
	}
	
	int* mouse_down_flag = getFlagByMouseType(button);
	if (mouse_down_flag != NULL)
	{
		*mouse_down_flag = is_mouse_down;
	}
	
    printf("Mouse click at %d %d, button: %d, state %d\n",x,y,button,state);
}


//This function is called whenever the mouse is moved with a mouse button held down.
// x and y are the location of the mouse (in window-relative coordinates)
void mouseMotion(int x, int y)
{
	int x_change = x - MOUSE_LAST_X;
	int y_change = y - MOUSE_LAST_Y;

	set_mouse_pos(x, y);

	if (LEFT_MOUSE_DOWN)
	{
		//Change look at
		double xdiff = x_change * translation_base;
		double ydiff = y_change * translation_base;
		//look_at = Coordinate::point3_non_homogeneous(look_at.x() + xdiff, look_at.y() + ydiff, look_at.z());
		update_state();
	}

	if (MIDDLE_MOUSE_DOWN)
	{
		//slide camera
		update_state();
	}

	if (RIGHT_MOUSE_DOWN)
	{
		//zoom
		update_state();
	}
	//printf("Mouse is at %d, %d\n", x,y);
}
	

void toggleFlag(int* flag)
{
	if (*flag)
	{
		*flag = OFF;
	}
	else
	{
		*flag = ON;
	}
}

void rotate_local(Matrix new_rotation)
{
	//Local multiply on right
	model_rotation = model_rotation * new_rotation;
}

void rotate_world(Matrix new_rotation)
{
	//world rotation multiply on left;
	world_rotation = new_rotation * world_rotation;
}

void translate_object(Matrix new_translation)
{
	//multiply a new translation on left
	model_translation = new_translation * model_translation;
}

void change_scale(int sign)
{
	scale = scale + scale_base * sign;
}

void set_camera_lookat()
{
	Coordinate zero = Coordinate::point3(0, 0, 0);
	if (NEXT_LOOKAT_IS_WORLD)
	{
		look_at = zero;
	}
	else
	{
		look_at = model_translation * zero;
	}

	toggleFlag(&NEXT_LOOKAT_IS_WORLD);
}

// This function is called whenever there is a keyboard input
// key is the ASCII value of the key pressed
// x and y are the location of the mouse

/*
"4" : negative translation along x axis
"6" : positive translation along x axis
"8" : positive translation along y axis
"2" : negative translation along y axis
"9" : positive translation along z axis
"1" : negative translation along z axis
"[" : negative rotation around x axis
"]" : positive rotation around x axis
";" : negative rotation around y axis
"'" : positive rotation around y axis
"." : negative rotation around z axis
"/" : positive rotation around z axis
"=" : increase uniform scaling
"-" : decrease uniform scaling

NOTE : the following are with respect to the LOCAL object coordinate system

"i" : negative rotation around local x axis
"o" : positive rotation around local x axis
"k" : negative rotation around local y axis
"l" : positive rotation around local y axis
"m" : negative rotation around local z axis
"," : positive rotation around local z axis

Other keyboard commands :

"a" : toggle display of coordinate axes, should display world and local object coordinate axes
"c" : snap camera to pointing at the world origin and pointing at the object at alternating order
"p" : toggle between perspective and orthogonal projection (already in the template)
"q" : exit program
*/
void keyboard(unsigned char key, int x, int y)
{
	printf("Key pressed %c\r\n", key);

    switch(key) {
	//WORLD TRANSFORMS
	//translation +,- | x,y,z
	case '6':
		translate_object(Matrix::translation(Coordinate::point3(translation_base, 0, 0)));
		break;
	case '4':
		translate_object(Matrix::translation(Coordinate::point3(-1 * translation_base, 0, 0)));
		break;
	case '8':
		translate_object(Matrix::translation(Coordinate::point3(0, translation_base, 0)));
		break;
	case '2':
		translate_object(Matrix::translation(Coordinate::point3(0, -1 * translation_base, 0)));
		break;
	case '9':
		translate_object(Matrix::translation(Coordinate::point3(0, 0, translation_base)));
		break;
	case '1':
		translate_object(Matrix::translation(Coordinate::point3(0, 0, -1 * translation_base)));
		break;
	//rotation -,+ x,y,z
	case '[':
		rotate_local(Matrix::rotation_x(-1 * rotation_base_angle));
		break;
	case ']':
		rotate_local(Matrix::rotation_x(rotation_base_angle));
		break;
	case ';':
		rotate_local(Matrix::rotation_y(-1 * rotation_base_angle));
		break;
	case '\'':
		rotate_local(Matrix::rotation_y(rotation_base_angle));
		break;
	case '.':
		rotate_local(Matrix::rotation_z(-1 * rotation_base_angle));
		break;
	case '/':
		rotate_local(Matrix::rotation_z(rotation_base_angle));
		break;
	//scaling +,-
	case '=':
		change_scale(1);
		break;
	case '-':
		change_scale(-1);
		break;

	//MODEL TRANSFORMS
	//rotation -,+ x,y,z
	case 'i':
	case 'I':
		rotate_world(Matrix::rotation_x(-1 * rotation_base_angle));
		break;
	case 'o':
	case 'O':
		rotate_world(Matrix::rotation_x(rotation_base_angle));
		break;
	case 'k':
	case 'K':
		rotate_world(Matrix::rotation_y(-1 * rotation_base_angle));
		break;
	case 'l':
	case 'L':
		rotate_world(Matrix::rotation_y(rotation_base_angle));
		break;
	case 'm':
	case 'M':
		rotate_world(Matrix::rotation_z(-1 * rotation_base_angle));
		break;
	case ',':
		rotate_world(Matrix::rotation_z(rotation_base_angle));
		break;


	//GENERAL CONTROL
	//quit
    case 'q':
	case 'Q':
		exit(1);
		break;
    case 'p':
    case 'P':
	// Toggle Projection Type (orthogonal, perspective)
		toggleFlag(&PERSPECTIVE);
		break;
	//axes
	case 'a':
	case 'A':
		toggleFlag(&SHOW_AXES);
		break;
	//Camera look at world orign / look at model origin
	case 'c':
	case 'C':
		set_camera_lookat();
		break;
    default:
		break;
    }

    // Schedule a new display event
	update_state();
}

// Here's the main
int main(int argc, char* argv[])
{
	//Initialize some empty transforms
	model_to_view = Matrix::scale(1);
	world_to_view = Matrix::scale(1);

    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Assignment 3");
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutKeyboardFunc(keyboard);

    // Initialize GL
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-2.5,2.5,-2.5,2.5,-10000,10000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);

	//Loade current transform settings
	//scaling by 1 is the same as identity
	model_rotation = Matrix::scale(1);
	model_translation = Matrix::translation(Coordinate::point3(0, 0, 0));
	world_rotation = Matrix::scale(1);
	camera_position = Coordinate::point3_non_homogeneous(0, 0, -30);
	up_vector = Coordinate::point3_non_homogeneous(0, 1, 0);
	look_at = Coordinate::point3_non_homogeneous(0, 0, 0);

	update_state();

    // Switch to main loop
    glutMainLoop();
    return 0;        
}
