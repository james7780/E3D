/*
 *		E3D OO Cross Platform 3D Lib
 *      Example 2: Load a scene
 *		James Higgs 2006
 *
 *		This Code based on Jeff Molofee's OpenGL template,
 *		and uses the "GL Framework Library" (GLFW) 
 */

#include "SDL.h"   /* All SDL App's need this */
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>

// NB: Include our E3D header file
#include "../../../include/E3D.h"
// NB: Set our C++ namespace prefix to E3D so we can more easily use E3D objects
using namespace E3D;

#define VIEW_WIDTH		640
#define VIEW_HEIGHT		480

int		mouse_x, mouse_y;
int		mouse_x_old, mouse_y_old;
int		mouse_dx, mouse_dy;

int		nTemp;
float	dyrot;
float	fwheel[4];

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


#define OBJ_MAX 400

class E3D_Scene Scene;
class E3D_Model gpModel[OBJ_MAX];
class E3D_Object gpObject[OBJ_MAX];

E3D_Utility util;


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
	//glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);		// set ambient light level
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);		// set light colour
	//glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);	// set light position or direction
	glEnable(GL_LIGHT1);								// Enable Light One
	glEnable(GL_LIGHTING);								// Enable Lighting
	glEnable(GL_NORMALIZE);								// fix normals after scaling

	return TRUE;										// Initialization Went OK
}

int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
	//int i;
	float scale = 10.0f;
	//float x, z;
	int steps = 50;
	//char stemp[512];
	E3D_Object *pO; 

	// Calculate a 3rd-person view
	pO = Scene.GetObject(0, 0);
	x_eye = (float)(-sin(yrot) * zoom + pO->position.x);
	z_eye = (float)(-cos(yrot) * zoom + pO->position.z);
	y_eye = pO->position.y + 10;

	// Set up the default scene camera
	E3D_Camera *camera = Scene.GetCamera(0);
	camera->SetPosition(x_eye, y_eye, z_eye);
	camera->LookAt(pO->position);

	// Tell E3D to render it's scene to the current GL context
	Scene.Render((float)VIEW_WIDTH, (float)VIEW_HEIGHT, true);

	return TRUE;						// Jump Back
}



/////////////////////////////////////////////////////////////////////////////////
// Main loop
/////////////////////////////////////////////////////////////////////////////////
int main(void) {

	bool done=FALSE;								// Bool Variable To Exit Loop
	int debounce = 0;

	int		running, frames, x, y;
    double  t, t0, fps;
    char    titlestr[ 200 ];
	int		scr_mode;

	// Initialise or scene
	Scene.Start();
	// Load our scene file
	Scene.Load("test.scn");

	// set the E3D scene background colour
	Scene.SetBackColour(0.5f, 0.3f, 0.2f);

   // Set up SDL video buffer
    printf("Initializing SDL.\n");
 
    // Information about the current video settings.
    const SDL_VideoInfo* info = NULL;
    // Color depth in bits of our window. 
    int bpp = 0;
    // Flags we will pass into SDL_SetVideoMode. 
    int flags = 0;

  
    // Initialize defaults, Video and Audio 
    if((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)==-1)) { 
        printf("Could not initialize SDL: %s.\n", SDL_GetError());
        exit(-1);
    }

    printf("SDL initialized.\n");

    // Let's get some video information. 
    info = SDL_GetVideoInfo( );

    if( !info ) {
        // This should probably never happen. 
        fprintf( stderr, "Video query failed: %s\n",
             SDL_GetError( ) );
        exit( 1 );
    }



    SDL_Surface *screen;

    // set quit cleanup callback
    atexit(SDL_Quit);

   /*
     * Set our width/height to 640/480 (you would
     * of course let the user decide this in a normal
     * app). We get the bpp we will request from
     * the display. On X11, VidMode can't change
     * resolution, so this is probably being overly
     * safe. Under Win32, ChangeDisplaySettings
     * can change the bpp.
     */
    bpp = info->vfmt->BitsPerPixel;

    /*
     * Now, we want to setup our requested
     * window attributes for our OpenGL window.
     * We want *at least* 5 bits of red, green
     * and blue. We also want at least a 16-bit
     * depth buffer.
     *
     * The last thing we do is request a double
     * buffered window. '1' turns on double
     * buffering, '0' turns it off.
     *
     * Note that we do not use SDL_DOUBLEBUF in
     * the flags to SDL_SetVideoMode. That does
     * not affect the GL attribute state, only
     * the standard 2D blitting setup.
     */
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

    /*
     * We want to request that SDL provide us
     * with an OpenGL window, in a fullscreen
     * video mode.
     *
     * EXERCISE:
     * Make starting windowed an option, and
     * handle the resize events properly with
     * glViewport.
     */
    flags = SDL_OPENGL; // | SDL_FULLSCREEN;

    /*
     * Set the video mode
     */
    if( SDL_SetVideoMode( VIEW_WIDTH, VIEW_HEIGHT, bpp, flags ) == 0 ) {
        /* 
         * This could happen for a variety of reasons,
         * including DISPLAY not being set, the specified
         * resolution not being available, etc.
         */
        fprintf( stderr, "Video mode set failed: %s\n",
             SDL_GetError( ) );
        exit( 1 );
    }

    SDL_WM_SetCaption("E3D Example 2 - Load Scene", "");

	// Set up our OpenGL env and build our display lists
	printf( "InitGL() returned %d\n", InitGL() );

    // Main loop
	dyrot = 0.0;
    running = GL_TRUE;
    frames = 0;
	unsigned int ticks = SDL_GetTicks();
    while( running ) {

		// limit fps to 50 frames per second
		while((SDL_GetTicks() - ticks) < 20) ;
		ticks = SDL_GetTicks();

		// get mouse position
        //glfwGetMousePos( &x, &y );

        frames ++;

		DrawGLScene();

	    // Swap the double-buffer
	    SDL_GL_SwapBuffers( );

        // Poll for the next event
        SDL_Event event;
        SDL_PollEvent( &event );

        // Check if the ESC key was pressed or the window was closed
        Uint8 *keys = SDL_GetKeyState(NULL);
        if ( keys[SDLK_ESCAPE] == SDL_PRESSED )
            {
            running = 0;
            }

		if(debounce) debounce--;
	} // wend

    printf("Quiting SDL.\n");
    
    // Shutdown all subsystems 
    SDL_Quit();
    
    exit(0);
}


