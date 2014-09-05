/*
 *		E3D Example 1 - Scene with 1 object
 *		James Higgs 2006
 *
 *		This Code based on Jeff Molofee's OpenGL template
 */

#include <windows.h>				// Header File For Windows
#include <stdio.h>					// Header File For Standard Input/Output
#include <math.h>
#include <gl\gl.h>					// Header File For The OpenGL32 Library
#include <gl\glu.h>					// Header File For The GLu32 Library
//#include <gl\glaux.h>				// Header File For The Glaux Library
#include "../../platforms\Win32.OpenGL.VS2008/common/glfw.h"			// Header File for the GL Framwork Lib

// NB: Include our E3D header file
#include "../../include/E3D.h"
// NB: Set our C++ namespace prefix to E3D so we can more easily use E3D objects
using namespace E3D;

#define VIEW_WIDTH		600
#define VIEW_HEIGHT		360

GLfloat zoom = 20;

class E3D_Scene Scene;

int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	printf("InitGL()\n");

	glShadeModel(GL_FLAT);								// FLAT / SMOOTH shading
	glClearColor(0.5f, 0.3f, 0.1f, 0.5f);				// Background colour
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
	glEnable(GL_LIGHT1);								// Enable Light One
	glEnable(GL_LIGHTING);								// Enable Lighting
	glEnable(GL_NORMALIZE);								// fix normals after scaling

	return TRUE;										// Initialization Went OK
}

int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
	Scene.Render((float)VIEW_WIDTH, (float)VIEW_HEIGHT, true);

	return TRUE;						// Jump Back
}


/////////////////////////////////////////////////////////////////////////////////
// Main loop
/////////////////////////////////////////////////////////////////////////////////
int main(void) {

	BOOL	done=FALSE;								// Bool Variable To Exit Loop
	int debounce = 0;

	int		running, frames, x, y;
    double  t, t0, fps;
    char    titlestr[200];
	int		scr_mode;
	E3D_Object *pObject;

	// set GLFW to windowed mode 
	scr_mode = GLFW_WINDOW;

	// Set up our scene
	Scene.Start();
	Scene.SetRenderDistance(400.0f);

	// NB: The scene contains 1 default light (light 0) after Scene.Start(),
	//     which we use in this example

	// The scene also contains 1 default camera (camera 0)
	E3D_Camera *camera = Scene.GetCamera(0);
	camera->SetPosition(0.0f, 5.0f, -40.0f);
	camera->SetDirection(0.0f, 0.0f, 1.0f);
	camera->SetUpVector(0.0f, 1.0f, 0.0f);
	camera->SetFOV(60.0f);

	// Add a 3D model to the scene
	//E3D_Model *model = Scene.AddModel("hwing", "hwing.obj");
	E3D_Model *model = Scene.AddModel("robot", "robot.obj");
	model->CalculateNormals();

	// Create a 3D object instance in the scene
	pObject = Scene.AddObject(0, "robot");
	pObject->SetPosition(0.0f, 0.0f, 0.0f);

	// make our camera look at the object
	camera->LookAt(pObject->position);

    // Initialise GLFW
    if(glfwInit() != GL_TRUE) return(1);

    // Open OpenGL window
    if( !glfwOpenWindow( VIEW_WIDTH, VIEW_HEIGHT, 0,0,0,0, 0,0, scr_mode ) )
    {
        glfwTerminate();
        return 0;
    }
    glfwSetWindowTitle( "E3DOO Win32OpenGL Example1" );

	//glfwSetWindowSizeCallback( ResizeWindow );

    // Disable vertical sync (on cards that support it)
    glfwSwapInterval( 1 );

	// Set up our OpenGL env and build our display lists
	printf( "InitGL() returned %d\n", InitGL() );

    // Swap buffers
    glfwSwapBuffers();

    // Main loop
    running = GL_TRUE;
    frames = 0;
    t0 = glfwGetTime();
	double startTime = t0;
    while( running ) {

		// limit fps to 50 frames per second
		while((glfwGetTime() - startTime) < 0.02) ;
		startTime = glfwGetTime();

		pObject->rotation.y += 0.05f;
		pObject->rotation.x += 0.03f;

        // Get time and mouse position
        t = glfwGetTime();
        glfwGetMousePos( &x, &y );

        // Calculate and display FPS (frames per second)
        if( (t-t0) > 1.0 || frames == 0 )
        {
            fps = (double)frames / (t-t0);
            sprintf_s(titlestr, 200, "E3DOO Win32OpenGL Example1 (%.1f FPS) %d objects", fps, Scene.GetNumObjects());
            glfwSetWindowTitle( titlestr );
            t0 = t;
            frames = 0;
        }
        frames ++;

		DrawGLScene();

        // Swap buffers
        glfwSwapBuffers();

        // Check if the ESC key was pressed or the window was closed
        running = !glfwGetKey( GLFW_KEY_ESC ) &&
                  glfwGetWindowParam( GLFW_OPENED );

		if(debounce) debounce--;
	} // wend

    // Close OpenGL window and terminate GLFW
    glfwTerminate();
	return 0;									// Exit The Program
}

