/*
 *		E3D Example 1 - Scene with 1 object
 *		James Higgs 2006
 *
 */

#include "SDL.h"   /* All SDL App's need this */
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>

// NB: Include our E3D header file
#include "../../../include/E3D.h"
// NB: Set our C++ namespace prefix to E3D so we can more easily use E3D objects
using namespace E3D;

#define VIEW_WIDTH		600
#define VIEW_HEIGHT		400

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

	bool done = false;								// Bool Variable To Exit Loop
	int debounce = 0;

	int		running, frames, x, y;
    double  t, t0, fps;
    char    titlestr[ 200 ];
	E3D_Object *pObject;

	// Set up our scene
	Scene.Start();
	Scene.SetRenderDistance(400.0f);

	// NB: The scene contains 1 default light (light 0) after Scene.Start(),
	//     which we use in this example

	// The scene also contains 1 default camera (camera 0)
	E3D_Camera *camera = Scene.GetCamera(0);
	camera->SetPosition(0.0f, 10.0f, -20.0f);
	camera->SetDirection(0.0f, 0.0f, 1.0f);
	camera->SetUpVector(0.0f, 1.0f, 0.0f);
	camera->SetFOV(60.0f);

	// Add a 3D model to the scene
	E3D_Model *model = Scene.AddModel("hwing", "hwing.obj");
	model->CalculateNormals();

	// Create a 3D object instance in the scene
	pObject = Scene.AddObject(0, "hwing");
	pObject->SetPosition(0.0f, 0.0f, 0.0f);

	// make our camera look at the object
	camera->LookAt(pObject->position);

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

    SDL_WM_SetCaption("E3D Example 1", "");

     /*
     * At this point, we should have a properly setup
     * double-buffered window for use with OpenGL.
     */
    InitGL();

    // Main loop
    running = GL_TRUE;
    frames = 0;
	unsigned int ticks = SDL_GetTicks();
    while( running ) {

		// limit fps to 50 frames per second
		while((SDL_GetTicks() - ticks) < 20) ;
		ticks = SDL_GetTicks();

		pObject->rotation.y += 0.05f;
		pObject->rotation.x += 0.03f;
/*
        // Get time and mouse position
        t = glfwGetTime();
        glfwGetMousePos( &x, &y );

        // Calculate and display FPS (frames per second)
        if( (t-t0) > 1.0 || frames == 0 )
        {
            fps = (double)frames / (t-t0);
            sprintf( titlestr, 
				"E3DOO Win32OpenGL Example1 (%.1f FPS) %d objects", 
				fps, Scene.GetNumObjects() );
            glfwSetWindowTitle( titlestr );
            t0 = t;
            frames = 0;
        }
        frames ++;
*/
        /* Draw the screen. */
        DrawGLScene();

	    /*
	     * Swap the buffers. This this tells the driver to
	     * render the next frame from the contents of the
	     * back-buffer, and to set all rendering operations
	     * to occur on what was the front-buffer.
	     *
	     * Double buffering prevents nasty visual tearing
	     * from the application drawing on areas of the
	     * screen that are being updated at the same time.
	     */
	    SDL_GL_SwapBuffers( );

        // Poll for the next event
        SDL_Event event;
        SDL_PollEvent( &event );

        Uint8 *keys = SDL_GetKeyState(NULL);
        if ( keys[SDLK_ESCAPE] == SDL_PRESSED )
            {
            printf("Bye bye...\n");
            exit(0);
            }

		if(debounce) debounce--;
	} // wend

    printf("Quiting SDL.\n");
    
    // Shutdown all subsystems 
    SDL_Quit();
    
    printf("Quiting....\n");

    exit(0);
}

