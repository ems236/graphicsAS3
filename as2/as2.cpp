// The template code for Assignment 2 
//

#include <stdio.h>
#include <stdlib.h>
#include <GL\glut.h>
#include <math.h>

#define ON 1
#define OFF 0
#define PI 3.14159265

// Global variables
int window_width, window_height;    // Window dimensions

//Current scene settings
int PERSPECTIVE = OFF;
int SHOW_AXES = ON;
int SHOW_OBJECT = ON;
int LEFT_MOUSE_DOWN = OFF;
int RIGHT_MOUSE_DOWN = OFF;
int MOUSE_LAST_X = NULL;
int MOUSE_LAST_Y = NULL;

//current camera settings
float camera_radius = 5.0;
//phi
float camera_latitude = PI / 2;
//theta
float camera_longitude = 0.0;

// Vertex and Face data structure sued in the mesh reader
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


void draw_axes()
{
	glBegin(GL_LINES);
		//x
		glColor3f(1, 0, 0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(50.0, 0.0, 0.0);

		//y
		glColor3f(0, 1, 0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 50.0, 0.0);

		//z
		glColor3f(0, 0, 1);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 50.0);
	glEnd();
}

void draw_object()
{

}

float max(float a, float b)
{
	if (a > b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

float clamp(float min, float max, float value)
{
	if (value < min)
	{
		return min;
	}
	if (value > max)
	{
		return max;
	}

	return value;
}

void change_latitude(int y_change)
{
	//modify phi
	camera_latitude = clamp(0, PI, (0.05 * y_change) + camera_latitude);
}

void change_longitude(int x_change)
{
	//modify theta
	camera_longitude = fmod((0.25 * x_change) + camera_longitude, 2 * PI);
}

void change_zoom(int y_change)
{
	//modify r
	camera_radius = max(0, (0.25 * y_change) + camera_radius);
}

point* camera_position()
{
	//Calc position around origin
	/*
	theta [0, 2pi] || [0, 360]
	phi [0, pi] || [0, 180]
	x = rcos(theta)sin(phi)
	y = rsin(theta)sin(phi)
	z = rcos(phi)
	*/
	point* camerapos = (point*)malloc(sizeof(point));

	camerapos->x = camera_radius * sinf(camera_latitude) * cosf(camera_longitude);
	camerapos->y = camera_radius * sinf(camera_latitude) * sinf(camera_longitude);
	camerapos->z = camera_radius * cosf(camera_latitude);

	return camerapos;
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

	if (PERSPECTIVE) {
		// Perpective Projection 
		gluPerspective(60, (GLdouble)window_width / window_height, 0.01, 10000);
	}
	else {
		// Orthogonal Projection 
		glOrtho(-2.5, 2.5, -2.5, 2.5, -10000, 10000);
	}

	glutSetWindowTitle("Assignment 2 Template (orthogonal)");
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Set the camera position, orientation and target
	gluLookAt(0, 0, camera_radius, 0, 0, 0, 0, 1, 0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Draw a red rectangle
    glColor3f(1,0,0);
	glBegin(GL_POLYGON);
		glVertex3f(0.8,0.8,-0.8);
		glVertex3f(0.8,-0.8,-0.8);
		glVertex3f(-0.8,-0.8,-0.0);
		glVertex3f(-0.8,0.8,-0.0);
    glEnd();

    // Draw a blue tetraheadron
    glColor3f(0,0,1);
    glBegin(GL_TRIANGLES);
		glVertex3f(0.0,1.6,0.0);
		glVertex3f(0.8,-0.4,0.8);
		glVertex3f(-0.8,-0.4,0.8);

		glVertex3f(0.0,1.6,0.0);
		glVertex3f(0.8,-0.4,0.8);
		glVertex3f(0.0,-0.4,-0.8);

		glVertex3f(0.0,1.6,0.0);
		glVertex3f(0.0,-0.4,-0.8);
		glVertex3f(-0.8,-0.4,0.8);

		glVertex3f(-0.8,-0.4,0.8);
		glVertex3f(0.8,-0.4,0.8);
		glVertex3f(0.0,-0.4,-0.8);
    glEnd();

	if (SHOW_AXES)
	{
		draw_axes();
	}

	if (SHOW_OBJECT)
	{
		draw_object();
	}

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

void check_reset_mouse_pos()
{
	if (LEFT_MOUSE_DOWN ^ RIGHT_MOUSE_DOWN)
	{
		MOUSE_LAST_X = NULL;
		MOUSE_LAST_Y = NULL;
	}
}

int* getFlagByMouseType(int mouseType)
{
	const int LEFT = 0;
	const int RIGHT = 2;

	if (mouseType == LEFT)
	{
		return &LEFT_MOUSE_DOWN;
	}
	if (mouseType == RIGHT)
	{
		return &RIGHT_MOUSE_DOWN;
	}
}

// This function is called whenever the mouse is pressed or released
// button is a number 0 to 2 designating the button
// state is 1 for release 0 for press event
// x and y are the location of the mouse (in window-relative coordinates)
void mouseButton(int button,int state,int x,int y)
{
	const int DOWN = 0;
	const int UP = 1;
	
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

	if (LEFT_MOUSE_DOWN)
	{
		change_longitude(x_change);
		change_latitude(y_change);
	}

	if (RIGHT_MOUSE_DOWN)
	{
		change_zoom(y_change);
	}
	printf("Mouse is at %d, %d\n", x,y);
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

// This function is called whenever there is a keyboard input
// key is the ASCII value of the key pressed
// x and y are the location of the mouse
void keyboard(unsigned char key, int x, int y)
{
	printf("Key pressed %c\r\n", key);

    switch(key) {
    case 'q':
	case 'Q':
		exit(1);
		break;
    case 'p':
    case 'P':
	// Toggle Projection Type (orthogonal, perspective)
		toggleFlag(&PERSPECTIVE);
		break;
	case 'a':
	case 'A':
		toggleFlag(&SHOW_AXES);
		break;
	case 's':
	case 'S':
		toggleFlag(&SHOW_OBJECT);
		break;
    default:
		break;
    }

    // Schedule a new display event
    glutPostRedisplay();
}

// Here's the main
int main(int argc, char* argv[])
{
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutCreateWindow("Assignment 2 Template (orthogonal)");
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

    // Switch to main loop
    glutMainLoop();
    return 0;        
}
