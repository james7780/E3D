/*
 *		E3D example 5: Battlezone
 *		James Higgs 2006
 *
 *		This Code based on Jeff Molofee's OpenGL template
 */

// TODO: playfield extents
// TODO: sights and score overlay

#include <windows.h>				// Header File For Windows
#include <stdio.h>					// Header File For Standard Input/Output
#include <math.h>
#include <gl\gl.h>					// Header File For The OpenGL32 Library
#include <gl\glu.h>					// Header File For The GLu32 Library
#include <gl\glaux.h>				// Header File For The Glaux Library
#include "../common/glfw.h"			// Header File for the GL Framwork Lib

// NB: Include our E3D header file
#include "../../../include/E3D.h"
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

// texture font print
void jprint(char *text, float x, float y) {
	int c;
	float tx, ty;
	char *pc;
	pc = text;

	while( *pc != 0 ) {
		c = *pc - 32;
		if(c < 0) c = 0;
		tx = float( c % 16 ) * 0.0625f;
		ty = float( c / 16 ) * 0.0625f; //125;
		glBegin(GL_QUADS);
			// normal
			glNormal3f(0.0f, 0.0f, -1.0f);
			// vertices
			glTexCoord2f( tx,  1.0f - ty );
			glVertex3f(x, y, 0);
			glTexCoord2f(  tx,  1.0f - ty - 0.0625f );
			glVertex3f(x, y+16, 0);
			glTexCoord2f(  tx + 0.0625f, 1.0f - ty - 0.0625f);
			glVertex3f(x+16, y+16, 0);
			glTexCoord2f( tx + 0.0625f, 1.0f - ty );
			glVertex3f(x+16, y, 0);
		glEnd();
		x += 12;
		pc++;
	}

}

int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
/*
	float scale = 10.0f;
	int steps = 50;

    // Enable scissor test
    glDisable( GL_SCISSOR_TEST );
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing

    // Main View
    glViewport( 0, 0, VIEW_WIDTH, VIEW_HEIGHT );
    glScissor( 0, 0, VIEW_WIDTH, VIEW_HEIGHT );

	//glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	//glLoadIdentity();									// Reset The Projection Matrix

	// glClear(), glPerspective(), gluLookAt() done in Scene.Render()

	glEnable(GL_LIGHTING);
*/
	// Tell E3D to render it's scene to the current GL context
	Scene.Render(VIEW_WIDTH, VIEW_HEIGHT, true);


	return TRUE;						// Jump Back
}

// add some fragments to the scene
void addFrags(Vector p, Vector v) {
	int i, j;
	// add 10 fragments
	for(i=0; i<10; i++) {
		for(j=10; j<=39; j++) {
			E3D_Object *pObject = Scene.GetObject(j);
			if(0 == pObject->behaviour) {
				pObject->behaviour = 1;
				pObject->counter = 40 + (rand() % 20);
				pObject->position = p;
				pObject->velocity.x = v.x + (rand() % 6 - 3) / 10.0f;
				pObject->velocity.y = v.y + (rand() % 6 - 3) / 10.0f;
				pObject->velocity.z = v.z + (rand() % 6 - 3) / 10.0f;
				pObject->rotation.y = rand() % 6;
				pObject->rotation.x = rand() % 6;
				j = 40;
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

	// Initialise the scene
	Scene.Start();
	Scene.SetRenderDistance(400.0f);

	// Set up the scene's default light (light 0)
	E3D_Light *light0 = Scene.GetLight(0);
	light0->SetColour( 0.8f, 0.8f, 0.6f );
	light0->SetAmbient( 0.4f, 0.4f, 0.4f );
	light0->SetDirection( 0.707f, 0.707f, 0.0f );

	// Set up the scene's default camera
	E3D_Camera *camera = Scene.GetCamera(0);
	camera->SetPosition(0.0f, 1.5f, 0.0f);
	camera->SetDirection(0.0f, 0.0f, 1.0f);
	camera->SetUpVector(0.0f, 1.0f, 0.0f);
	camera->SetFOV(60.0f);

	Scene.AddModel("block", "block.obj");
	Scene.AddModel("ufo", "ufo.obj");
	Scene.AddModel("tank", "tank.obj");
	Scene.AddModel("laser_yellow", "laser_yellow.obj");
	Scene.AddModel("laser_green", "laser_green.obj");
	Scene.AddModel("frag", "frag.obj");

	// add sights
	pObject = Scene.AddObject("block");
	pObject->SetPosition(0.0f, 0.0f, 100.0f);

	// add some bullets
	for(i=1; i<9; i++) {
		pObject = Scene.AddObject("laser_yellow");
		pObject->SetPosition(0.0f, 1000.0f, 0.0f);				// offscreen
	}

	// add some fragments (objects 10 to 50)
	for(i=10; i<40; i++) {
		pObject = Scene.AddObject("frag");
		pObject->SetPosition(0.0f, 1000.0f, 0.0f);				// offscreen
	}

	// manufacture some blocks
	for(i=40; i<80; i++) {
		pObject = Scene.AddObject("block");
		pObject->SetPosition(rand() % 500 - 250, 0.0, rand() % 500 - 250);
	}

	// manufacture some tanks
	for(i=80; i<90; i++) {
		pObject = Scene.AddObject("tank");
		pObject->SetPosition(rand() % 500 - 250, 0.0f, rand() % 500 - 250);
		pObject->behaviour = 1;
	}

	// manufacture some ufos
	for(i=90; i<100; i++) {
		pObject = Scene.AddObject("ufo");
		pObject->SetPosition(rand() % 500 - 250, 10.0f, rand() % 500 - 250);
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
    glfwSetWindowTitle( "E3DOO Win32OpenGL Example5" );

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
	float heading = 0.0;
    running = GL_TRUE;
    frames = 0;
    t0 = glfwGetTime();
	double startTime = t0;
	glfwDisable(GLFW_MOUSE_CURSOR);
    while( running ) {

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
		/*
		dz += 0.05f;
		pObject = Scene.GetObject(0);
		pObject->rz = dz;
		pObject->x = (float)((VIEW_WIDTH/2) - x) / 40.0f;
		pObject->y = (float)((VIEW_HEIGHT/2) - y) / 40.0f;
		*/
		for(i=1; i<Scene.GetNumObjects(); i++) {
			pObject = Scene.GetObject(i);
			switch(pObject->GetModelNumber()) {
				case 1 : // this is a ufo
					if(1 == pObject->behaviour) {
						pObject->rotation.y += 0.1f;
						pObject->position.z -= 0.2f;
						if(pObject->position.z < -250.0f) {
							pObject->position.z = 200.0f + rand() % 50;
						}
					}
					break;
				case 2 : // tank
					if(1 == pObject->behaviour) {
						pObject->rotation.y += 0.01f;
						//pObject->position.z -= 1.0f;
					}
					break;
				case 3 : // bullet (yellow laser)
					if(1 == pObject->behaviour)	{	// active
						pObject->position += pObject->velocity;
						// check if this bullet has hit any tanks or ufos
						for(j=40; j<Scene.GetNumObjects(); j++) {
							E3D_Object *pTarget = Scene.GetObject(j);
							if(fabs(pTarget->position.z - pObject->position.z) < 5.0f) {
								if(fabs(pTarget->position.x - pObject->position.x) < 5.0f) {
									if(fabs(pTarget->position.y - pObject->position.y) < 5.0f) {
										// hit!
										Vector v(0.0f, 1.0f,0.0f);
										addFrags(pTarget->position, v);
										pTarget->SetPosition(0.0f, 1000.0f, 0.0f);
										pTarget->behaviour = 0;		// switch off glider
										pObject->counter = 1;		// expire bullet
									}
								}
								
							}
						}
						if(pObject->counter) pObject->counter--;	// decrement counter
						if(!pObject->counter) {
							pObject->behaviour = 0;	// switch off if finished count
							pObject->SetPosition(0.0f, 1000.0f, 0.0f);
						}
					}
					break;
				case 4 : // (green laser)
					break;
				case 5 : // fragment
					if(1 == pObject->behaviour) {
						pObject->position += pObject->velocity;
						if(pObject->position.y < 0.0)
							pObject->velocity.y *= -1.0f;			// bounce off floor
						pObject->velocity.y -= 0.05f;				// gravity
						pObject->rotation.x += pObject->velocity.x * 0.1f;
						pObject->rotation.y += pObject->velocity.y * 0.1f;
						pObject->rotation.z += pObject->velocity.z * 0.1f;
						if(pObject->counter) pObject->counter--;	// decrement counter
						if(!pObject->counter) {
							pObject->behaviour = 0;	// switch off if finished count
							pObject->SetPosition(0.0f, 1000.0f, 0.0f);
						}
					}
					break;
			}

		}

		DrawGLScene();

        // Swap buffers
        glfwSwapBuffers();

        // Check if the ESC key was pressed or the window was closed
        running = !glfwGetKey( GLFW_KEY_ESC ) &&
                  glfwGetWindowParam( GLFW_OPENED );

		if(glfwGetKey(GLFW_KEY_LEFT))
			heading += 0.03f;
		else if(glfwGetKey(GLFW_KEY_RIGHT))
			heading -= 0.03f;

		Vector dirn((float)sin(heading), 0.0f, (float)cos(heading));
		camera->SetDirection(dirn);

		Vector posn = camera->GetPosition();
		if(glfwGetKey(GLFW_KEY_UP)) {
			posn.x += dirn.x * 0.5f;
			posn.z += dirn.z * 0.5f;
			camera->SetPosition(posn);
		}
		else if(glfwGetKey(GLFW_KEY_DOWN)) {
			posn.x -= dirn.x * 0.5f;
			posn.z -= dirn.z * 0.5f;
			camera->SetPosition(posn);
		}

		if(glfwGetKey(GLFW_KEY_SPACE)) {
			if(0 == debounce) {
				// find first free bullet and launch it
				for(i=1; i<7; i++) {
					pObject = Scene.GetObject(i);
					if(0 == pObject->behaviour) {
						pObject->behaviour = 1;
						pObject->counter = 100;
						pObject->SetPosition(posn.x, 2.0f, posn.z);
						pObject->SetVelocity(dirn.x * 1.0f, 0.0f, dirn.z * 1.0f);
						pObject->rotation.y = heading;
						i = 8;
					}
				}
				debounce = 12;
			}
		}

		if(debounce) debounce--;
	} // wend

    // Close OpenGL window and terminate GLFW
	glfwEnable(GLFW_MOUSE_CURSOR);
    glfwTerminate();
	return 0;									// Exit The Program
}

