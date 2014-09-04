/*
 *		E3D example 4: Tailgunner
 *		James Higgs 2006
 *
 *		This Code based on Jeff Molofee's OpenGL template
 */

#include <windows.h>				// Header File For Windows
#include <stdio.h>					// Header File For Standard Input/Output
#include <math.h>
#include <gl\gl.h>					// Header File For The OpenGL32 Library
//#include <gl\glu.h>					// Header File For The GLu32 Library
//#include <gl\glaux.h>				// Header File For The Glaux Library
#include "../../platforms\Win32.OpenGL.VS2008/common/glfw.h"			// Header File for the GL Framwork Lib

// NB: Include our E3D header file
#include "../../include/E3D.h"
// NB: Set our C++ namespace prefix to E3D so we can more easily use E3D objects
using namespace E3D;

#define VIEW_WIDTH		600
#define VIEW_HEIGHT		400

bool	active=TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen=TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default
bool	light;				// Lighting ON/OFF ( NEW )
bool	lp;					// L Pressed? ( NEW )

GLuint	texture[1];			// Storage For 1 Texture
GLuint	texid;
GLuint	model1;				// Storage For The Box Display List
GLuint	top;				// Storage For The Top Display List
GLuint	displist[20];
GLuint	xloop;				// Loop For X Axis
GLuint	yloop;				// Loop For Y Axis

GLfloat	xrot;				// Rotates Cube On The X Axis
GLfloat	yrot;				// Rotates Cube On The Y Axis
GLfloat zrot = 0.0f;
GLfloat zoom = 50;
GLfloat prop_rot;
GLfloat xpos[50], ypos[50], zpos[50];
GLfloat x_tree[200], y_tree[200], z_tree[200];
GLfloat z_gnd;
GLfloat y_eye = 50.0f;
GLfloat z_eye = -80.0f;
GLfloat x_eye = 0.0f;
GLfloat treescale = 1.0f;
GLfloat LightAmbient[]= { 0.2f, 0.2f, 0.2f, 1.0f }; 			// Ambient Light Values
GLfloat LightDiffuse[]= { 0.8f, 0.8f, 0.6f, 1.0f };				// Diffuse Light Values 
GLfloat LightPosition[]= { 0.0f, 0.7f, 0.7f, 0.0f };			// Light Position ( NEW )


int		mouse_x, mouse_y;
int		mouse_x_old, mouse_y_old;
int		mouse_dx, mouse_dy;

int		nTemp;
float	dyrot;
float	fwheel[4];



#define OBJ_MAX 400

class E3D_Scene Scene;
class E3D_Model gpModel[OBJ_MAX];
class E3D_Object gpObject[OBJ_MAX];

E3D_Utility util;

// object list defines
enum {
	PLAYER_LIST = 0,
	BULLET_LIST = 1,
	ENEMY_LIST = 2,
	FRAG_LIST = 3
};

void GLFWCALL ResizeWindow( int width, int height ) {
	//glfwSetWindowSize( width, height );

	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	// JH - changed far z here to 500
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,1.0f,500.0f);

	// set camera position and direction
	//gluLookAt(0, 20, 0, 0, 0, 50, 0, 1, 0); 

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix

}


int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	printf("InitGL()\n");

	glShadeModel(GL_FLAT);								// FLAT / SMOOTH shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Background colour
	glClearDepth(1.0f);									// Depth Buffer Setup

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LESS);								// The Type Of Depth Testing To Do
	//glDepthRange(0.5, 200.0);
	glEnable(GL_COLOR_MATERIAL);						// Enable Material Coloring
	//glEnable(GL_POLYGON_SMOOTH);

	//glPolygonMode(GL_FRONT, GL_LINE);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	//glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	// lighting
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);		// set ambient light level
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);		// set light colour
	glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);	// set light position or direction
	glEnable(GL_LIGHT1);								// Enable Light One
	glEnable(GL_LIGHTING);								// Enable Lighting
	glEnable(GL_NORMALIZE);								// fix normals after scaling

	return TRUE;										// Initialization Went OK
}


int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
	// Tell E3D to render it's scene to the current GL context
	Scene.Render(VIEW_WIDTH, VIEW_HEIGHT, true);


	return TRUE;						// Jump Back
}


// add some fragments to the scene
void addFrags(Vector p, Vector v) {
	int i, j;
	// add 10 fragments
	for(i=0; i<10; i++) {
		for(j=0; j<=29; j++) {
			E3D_Object *pObject = Scene.GetObject(FRAG_LIST, j);
			if(0 == pObject->behaviour) {
				pObject->behaviour = 1;
				pObject->counter = 50;
				pObject->position = p;
				pObject->velocity.x = v.x + (rand() % 6 - 3) / 10.0f;
				pObject->velocity.y = v.y + (rand() % 6 - 3) / 10.0f;
				pObject->velocity.z = v.z + (rand() % 6 - 3) / 10.0f;
				pObject->SetRotation((float)(rand() % 6), (float)(rand() % 6), 0.0f);
				j = 30;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////////
// Main loop
/////////////////////////////////////////////////////////////////////////////////
int main(void) {

	BOOL	done=FALSE;								// Bool Variable To Exit Loop
	short i, j;
	//char stemp[256];
	int debounce = 0;

    //int     width;
	//int		height;
	int		running, frames, x, y;
    double  t, t0, fps;
    char    titlestr[ 200 ];
	int		scr_mode;
	E3D_Object *pObject;


	scr_mode = GLFW_WINDOW;

	Scene.Start();
	Scene.SetRenderDistance(400.0f);

	// Set up the default light (light 0)
	E3D_Light *light0 = Scene.GetLight(0);
	light0->SetColour( 0.8f, 0.8f, 0.6f );
	light0->SetAmbient( 0.4f, 0.4f, 0.4f );
	light0->SetDirection( 0.707f, 0.707f, 0.0f );

	// Set up the default camera (camera 0)
	E3D_Camera *camera = Scene.GetCamera(0);
	camera->SetPosition(0.0f, 0.0f, 0.0f);
	camera->SetDirection(0.0f, 0.0f, 1.0f);
	camera->SetUpVector(0.0f, 1.0f, 0.0f);
	camera->SetFOV(60.0f);

	Scene.AddModel("sights", "sights.obj");
	Scene.AddModel("glider", "glider.obj");
	Scene.AddModel("frag", "frag.obj");
	Scene.AddModel("laser_yellow", "laser_yellow.obj");
	Scene.AddModel("laser_green", "laser_green.obj");

	// add sights
	pObject = Scene.AddObject(PLAYER_LIST, "sights");
	pObject->SetPosition(0.0f, 0.0f, 10.0f);

	// add some bullets
	for(i=0; i<8; i++) {
		pObject = Scene.AddObject(BULLET_LIST, "laser_yellow");
		pObject->SetPosition(50.0f, 0.0f, 0.0f);				// offscreen
	}

	// add some fragments (objects 10 to 50)
	for(i=0; i<30; i++) {
		pObject = Scene.AddObject(FRAG_LIST, "frag");
		pObject->SetPosition(50.0f, 0.0f, 0.0f);				// offscreen
	}

	// manufacture some gliders
	for(i=0; i<160; i++) {
		pObject = Scene.AddObject(ENEMY_LIST, "glider");
		pObject->SetPosition((float)(rand() % 100 - 50), (float)(rand() % 80 - 40), 50.0f + (float)(rand() % 300));
		pObject->behaviour = 1;
	}

    // Initialise GLFW
    if(glfwInit() != GL_TRUE) return(1);

    // Open OpenGL window
    if( !glfwOpenWindow( VIEW_WIDTH, VIEW_HEIGHT, 0,0,0,0, 0,0, scr_mode ) )
    {
        glfwTerminate();
        return 0;
    }
    glfwSetWindowTitle( "E3DOO Win32OpenGL Example1" );

	glfwSetWindowSizeCallback( ResizeWindow );

	//printf("Refresh rate: %d \n", glfwGetWindowParam(GLFW_REFRESH_RATE));
	//printf("ext1: %d \n", glfwExtensionSupported("GL_ARB_multitexture"));

    // Disable vertical sync (on cards that support it)
    glfwSwapInterval( 1 );

	// Set up our OpenGL env and build our display lists
	printf( "InitGL() returned %d\n", InitGL() );

    // Swap buffers
    glfwSwapBuffers();

	float dx = 0.5f;
	float dy = 0.2f;
	float dz = 0.0f;

    // Main loop
	dyrot = 0.0;
    running = GL_TRUE;
    frames = 0;
    t0 = glfwGetTime();
	double startTime = t0;
	glfwDisable(GLFW_MOUSE_CURSOR);
    while( running )
		{
		// fps limiter
		while((glfwGetTime() - startTime) < 0.02) ;
		startTime = glfwGetTime();

        // Get time and mouse position
        t = glfwGetTime();
        glfwGetMousePos( &x, &y );

        // Calculate and display FPS (frames per second)
        if( (t-t0) > 1.0 || frames == 0 )
			{
            fps = (double)frames / (t-t0);
            sprintf( titlestr, 
				"E3DOO Win32OpenGL Example1 (%.1f FPS) %d objects", 
				fps, Scene.GetNumObjects());
            glfwSetWindowTitle( titlestr );
            t0 = t;
            frames = 0;
			}
        frames ++;

		// animate
		dz += 0.05f;
		pObject = Scene.GetObject(0, 0);		// sights
		pObject->rotation.z = dz;
		pObject->position.x = (float)((VIEW_WIDTH/2) - x) / 40.0f;
		pObject->position.y = (float)((VIEW_HEIGHT/2) - y) / 40.0f;

		// BEGIN BEHAVIOURS
		// Behaviours 1: Enemy objects (gliders)
		OBJLIST* objList = Scene.GetObjectList(ENEMY_LIST);
		OBJLIST::iterator objIter;
		for(objIter = objList->begin(); objIter != objList->end(); objIter++)
			{
			if(1 == objIter->modelNumber)
				{
				// this is a glider
				if(1 == objIter->behaviour)
					{
					objIter->SetRotation(0.0f, DEG2RAD(180), 0.0f);
					objIter->position.z -= 1.0f;
					if(objIter->position.z < 0.0f)
						objIter->position.z = 300.0f + rand() % 50;
					}
				}
			}

		// Behaviours 2: Bullets
		objList = Scene.GetObjectList(BULLET_LIST);
		for(objIter = objList->begin() ; objIter != objList->end(); objIter++)
			{
			if(3 == objIter->modelNumber)
				{
				if(1 == objIter->behaviour)	// active
					{
					objIter->position += objIter->velocity;
					// check if this bullet has hit any gliders
					OBJLIST* gliderList = Scene.GetObjectList(ENEMY_LIST);
					OBJLIST::iterator gliderIter;
					for(gliderIter = gliderList->begin() ; gliderIter != gliderList->end(); gliderIter++)
						{
						if(1 == gliderIter->behaviour)	// glider active
							{
							if(fabs(gliderIter->position.z - objIter->position.z) < 5.0f)
								{
								if(fabs(gliderIter->position.x - objIter->position.x) < 5.0f)
									{
									if(fabs(gliderIter->position.y - objIter->position.y) < 5.0f)
										{
										// hit!
										Vector v(0.0f, 0.0f, -1.0f);
										addFrags(gliderIter->position, v);
										gliderIter->position.z = 0.0f;
										gliderIter->position.x = 50.0f;
										gliderIter->behaviour = 0;		// switch off glider
										objIter->counter = 1;		// expire bullet
										}
									}
								}
							}
						}	// next glider

					if(objIter->counter) objIter->counter--;	// decrement counter
					if(!objIter->counter)
						{
						objIter->behaviour = 0;	// switch off if finished count
						objIter->position.z = 0.0f;
						}
					}
				}	// end if bullet
			} // next

		// Behaviours 3: Fragments
		objList = Scene.GetObjectList(FRAG_LIST);
		for(objIter = objList->begin() ; objIter != objList->end(); objIter++)
			{
			if(2 == objIter->modelNumber)
				{
				if(1 == objIter->behaviour)
					{
					objIter->position += objIter->velocity;
					objIter->rotation += (objIter->velocity * 0.1f);
					if(objIter->counter) objIter->counter--;	// decrement counter
					if(!objIter->counter)
						{
						objIter->behaviour = 0;	// switch off if finished count
						objIter->position.x = 50.0f;
						objIter->position.z = 0.0f;
						}
					}
				}
			}
		// END BEHAVIOURS

		DrawGLScene();

        // Swap buffers
        glfwSwapBuffers();

        // Check if the ESC key was pressed or the window was closed
        running = !glfwGetKey( GLFW_KEY_ESC ) && glfwGetWindowParam( GLFW_OPENED );

		// get Mouse Button
		if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			{
			if(0 == debounce)
				{
				// find first free bullet and launch it
				OBJLIST* bulletList = Scene.GetObjectList(BULLET_LIST);
				OBJLIST::iterator bulletIter;
				//for(bulletIter = bulletList.begin(); bulletIter != bulletList.end(); bulletIter++)
				for(bulletIter = bulletList->begin(); bulletIter != bulletList->end(); bulletIter++)
					{
					if(0 == bulletIter->behaviour)
						{
						bulletIter->behaviour = 1;
						bulletIter->counter = 100;
						bulletIter->SetPosition(-1.0f, -1.0f, 0.0f);
						bulletIter->velocity.x = (float)((VIEW_WIDTH/2) - x) / 200.0f;
						bulletIter->velocity.y = (float)((VIEW_HEIGHT/2) - y) / 200.0f;
						bulletIter->velocity.z = 2.0f;
						bulletIter->rotation.y = (float)((VIEW_WIDTH/2) - x) / 400.0f;
						bulletIter->rotation.x = (float)((VIEW_HEIGHT/2) - y) / -400.0f;
						//bulletIter = bulletList->end();
						break;
						}
					}
				debounce = 12;
				}
			}

		if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
			{
			}

		if(debounce) debounce--;
		} // wend

    // Close OpenGL window and terminate GLFW
	glfwEnable(GLFW_MOUSE_CURSOR);
    glfwTerminate();
	return 0;									// Exit The Program
}

