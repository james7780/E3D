/*
 *		E3D example 4: Tailgunner
 *		James Higgs 2006
 *
 *		This Code based on Jeff Molofee's OpenGL template
 */

#include "SDL.h"   /* All SDL App's need this */
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>

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

	bool done=FALSE;								// Bool Variable To Exit Loop
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

    SDL_WM_SetCaption("E3D Example 4 - Tailgunner", "");

	// Set up our OpenGL env and build our display lists
	printf( "InitGL() returned %d\n", InitGL() );

	float dx = 0.5f;
	float dy = 0.2f;
	float dz = 0.0f;

    // Main loop
	dyrot = 0.0;
    running = GL_TRUE;
    frames = 0;
	unsigned int ticks = SDL_GetTicks();
    while( running )
		{
		// limit fps to 50 frames per second
		while((SDL_GetTicks() - ticks) < 20) ;
		ticks = SDL_GetTicks();

        // Get mouse position
		//SDL_PumpEvents();
		Uint8 mousebutton = SDL_GetMouseState( &x, &y );

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

		// get Mouse Button
		if(mousebutton & SDL_BUTTON(1))
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
			} // end if mousebutton

		if(debounce) debounce--;
		} // wend

    printf("Quiting SDL.\n");
    
    // Shutdown all subsystems 
    SDL_Quit();
    
    exit(0);
}

