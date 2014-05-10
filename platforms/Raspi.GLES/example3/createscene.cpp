/*
 *		E3D example3 - Create scene programatically and animate
 *		James Higgs 2006
 *
 *		This Code based on Jeff Molofee's OpenGL template
 */

#include "SDL.h"   /* All SDL App's need this */
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>

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
GLfloat zoom = 10;
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

int InitGL()										// All Setup For OpenGL Goes Here
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
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);		// set ambient light level
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);		// set light colour
	glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);	// set light position or direction
	glEnable(GL_LIGHT1);								// Enable Light One
	glEnable(GL_LIGHTING);								// Enable Lighting
	glEnable(GL_NORMALIZE);								// fix normals after scaling

	return TRUE;										// Initialization Went OK
}


int DrawGLScene()									// Here's Where We Do All The Drawing
{
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix

	// switch off texturing
	glBindTexture(GL_TEXTURE_2D, 0);

	glShadeModel(GL_FLAT);								// FLAT / SMOOTH shading

	// NB: it seems like you have to specify your xformations in reverse order (!!!)
	// !!!wierd!!!
	// draw objects in scene
	// TODO: cull non-visible objects
	glEnable(GL_LIGHTING);

	E3D_Object *pO; 

	// 3rd-person view
	pO = Scene.GetObject(0, 0);
	x_eye = (float)(-sin(yrot) * zoom + pO->position.x);
	z_eye = (float)(-cos(yrot) * zoom + pO->position.z);
	y_eye = pO->position.y + 5.0f;

	// set up the default camera
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
	//short i;
	//char stemp[256];
	int debounce = 0;

    //int     width;
	//int		height;
	int		running, frames, x, y;
    double  t, t0, fps;
    char    titlestr[ 200 ];
	int		scr_mode;
	E3D_Object *pObject;


	// Reset the scene
	Scene.Start();
	Scene.SetRenderDistance(400.0f);

	// Add the required models to the scene
	Scene.AddModel("carbody", "stcar.3do");
	Scene.AddModel("wheel", "wheel2.3do");
	Scene.AddModel("tree", "tree.3do");

	// add a car to the scene, then add some wheels to the car
	pObject = Scene.AddObject(0, "carbody");
	pObject->SetPosition(0.0f, 0.0f, -300.0f);
	pObject = Scene.AddObject(0, "wheel");		// front left
	pObject->SetPosition(1.0f, 0.0f, 1.2f);
	Scene.AttachObject(0, 1, 0);
	pObject = Scene.AddObject(0, "wheel");		// front right
	pObject->SetPosition(-1.0f, 0.0f, 1.2f);
	Scene.AttachObject(0, 2, 0);
	pObject = Scene.AddObject(0, "wheel");		// rear left
	pObject->SetPosition(1.0f, 0.0f, -1.0f);
	Scene.AttachObject(0, 3, 0);
	pObject = Scene.AddObject(0, "wheel");		// rear right
	pObject->SetPosition(-1.0f, 0.0f, -1.0f);
	Scene.AttachObject(0, 4, 0);

	// finally, add a few trees to the scene
	for(int i=0; i<30; i++) {
		pObject = Scene.AddObject(1, "tree");		// trees
		pObject->position.x = (float)(rand()%400 - 200);
		pObject->position.z = (float)(rand()%400 - 200);
		pObject->SetScale(2, 2, 2);
	}

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

    SDL_WM_SetCaption("E3D Example 3", "");

	// Set up our OpenGL env and build our display lists
	printf( "InitGL() returned %d\n", InitGL() );

    // Main loop
	dyrot = 0.0;
    running = GL_TRUE;
    frames = 0;
 	unsigned int ticks = SDL_GetTicks();
    while( running ) {

		// limit fps to 60 frames per second
		while((SDL_GetTicks() - ticks) < 16) ;
		ticks = SDL_GetTicks();

        // Get tmouse position
        //glfwGetMousePos( &x, &y );

        frames ++;

		// animate scene (move car and rotate it's wheels)
		yrot += DEG2RAD(0.2f);
		Scene.GetObject(0, 0)->position.z += 0.4f;
		for(int i=1; i < 5; i++) {
			Scene.GetObject(0, i)->rotation.x += DEG2RAD(6.0f);
		}

		DrawGLScene();

        // Swap buffers (double-buffering)
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

