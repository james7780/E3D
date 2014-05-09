/*
 *		E3D example 5: Battlezone
 *		James Higgs 2006
 *
 *		This Code based on Jeff Molofee's OpenGL template
 */

// TODO: playfield extents
// TODO: sights and score overlay

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
	PLAYER_LIST	= 0,
	BULLET_LIST = 1,
	ENEMY_LIST	= 2,
	FRAG_LIST	= 3
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
	// Tell E3D to render it's scene to the current GL context
	Scene.Render(VIEW_WIDTH, VIEW_HEIGHT, true);

	return TRUE;						// Jump Back
}

// add some fragments to the scene
void addFrags(Vector p, Vector v) {
	// add 10 fragments
	OBJLIST* objList = Scene.GetObjectList(FRAG_LIST);
	for(int i = 0; i < 10; i++)
		{
		OBJLIST::iterator iter;
		for(iter = objList->begin(); iter != objList->end(); iter++)
			{
			if(0 == iter->behaviour)
				{
				iter->behaviour = 1;
				iter->counter = 40 + (rand() % 20);
				iter->position = p;
				iter->velocity.x = v.x + (rand() % 6 - 3) / 10.0f;
				iter->velocity.y = v.y + (rand() % 6 - 3) / 10.0f;
				iter->velocity.z = v.z + (rand() % 6 - 3) / 10.0f;
				iter->rotation.y = rand() % 6;
				iter->rotation.x = rand() % 6;
				break;
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
	pObject = Scene.AddObject(PLAYER_LIST, "block");
	pObject->SetPosition(0.0f, 0.0f, 100.0f);

	// add some bullets
	for(i=0; i<8; i++) {
		pObject = Scene.AddObject(BULLET_LIST, "laser_yellow");
		pObject->SetPosition(0.0f, 1000.0f, 0.0f);				// offscreen
	}

	// add some fragments (objects 10 to 50)
	for(i=0; i<30; i++) {
		pObject = Scene.AddObject(FRAG_LIST, "frag");
		pObject->SetPosition(0.0f, 1000.0f, 0.0f);				// offscreen
	}

	// manufacture some blocks
	for(i=0; i<40; i++) {
		pObject = Scene.AddObject(ENEMY_LIST, "block");
		pObject->SetPosition(rand() % 500 - 250, 0.0, rand() % 500 - 250);
	}

	// manufacture some tanks
	for(i=0; i<10; i++) {
		pObject = Scene.AddObject(ENEMY_LIST, "tank");
		pObject->SetPosition(rand() % 500 - 250, 0.0f, rand() % 500 - 250);
		pObject->behaviour = 1;
	}

	// manufacture some ufos
	for(i=0; i<10; i++) {
		pObject = Scene.AddObject(ENEMY_LIST, "ufo");
		pObject->SetPosition(rand() % 500 - 250, 10.0f, rand() % 500 - 250);
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
     * with an OpenGL window
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

    SDL_WM_SetCaption("E3D Example 5 - Battlezone", "");

	// Set up our OpenGL env and build our display lists
	printf( "InitGL() returned %d\n", InitGL() );

	float dx = 0.5f;
	float dy = 0.2f;
	float dz = 0.0f;

    // Main loop
	float heading = 0.0;
    running = GL_TRUE;
    frames = 0;
	unsigned int ticks = SDL_GetTicks();
    while( running ) {

		// limit fps to 50 frames per second
		while((SDL_GetTicks() - ticks) < 20) ;
		ticks = SDL_GetTicks();

        frames ++;

		// animate
		/*
		dz += 0.05f;
		pObject = Scene.GetObject(0);
		pObject->rz = dz;
		pObject->x = (float)((VIEW_WIDTH/2) - x) / 40.0f;
		pObject->y = (float)((VIEW_HEIGHT/2) - y) / 40.0f;
		*/

		// BEGIN BEHAVIOURS
		// Behaviours 1: Enemy objects (tanks, ufos and blocks)
		OBJLIST* objList = Scene.GetObjectList(ENEMY_LIST);
		OBJLIST::iterator objIter;
		for(objIter = objList->begin(); objIter != objList->end(); objIter++)
			{
			if(1 == objIter->modelNumber)			// UFO
				{
				if(1 == objIter->behaviour)
					{
					objIter->rotation.y += 0.1f;
					objIter->position.z -= 0.2f;
					if(objIter->position.z < -250.0f)
						{
						objIter->position.z = 200.0f + rand() % 50;
						}
					}
				}
			else if(2 == objIter->modelNumber)			// TANK
				{
				if(1 == objIter->behaviour)
					{
					objIter->rotation.y += 0.01f;
					//objIter->position.z -= 1.0f;
					}
				}
			else if(0 == objIter->modelNumber)			// BLOCK
				{
				// do nothing
				}
			}

		// 2. Bullet behaviour
		objList = Scene.GetObjectList(BULLET_LIST);
		for(objIter = objList->begin(); objIter != objList->end(); objIter++)
			{
			if(3 == objIter->modelNumber)			// YELLOW LASER
				{
				if(1 == objIter->behaviour)
					{
					objIter->position += objIter->velocity;
					// check if this bullet has hit any tanks or ufos
					OBJLIST* enemyList = Scene.GetObjectList(ENEMY_LIST);
					OBJLIST::iterator enemyIter;
					for(enemyIter = enemyList->begin() ; enemyIter != enemyList->end(); enemyIter++)
						{
						if(1 == enemyIter->behaviour)	// enemy active
							{
							if(fabs(enemyIter->position.z - objIter->position.z) < 5.0f)
								{
								if(fabs(enemyIter->position.x - objIter->position.x) < 5.0f)
									{
									if(fabs(enemyIter->position.y - objIter->position.y) < 5.0f)
										{
										// hit!
										Vector v(0.0f, 1.0f,0.0f);
										addFrags(enemyIter->position, v);
										enemyIter->SetPosition(0.0f, 1000.0f, 0.0f);
										enemyIter->behaviour = 0;		// switch off glider
  										objIter->counter = 1;		// expire bullet
										}
									}
								}
							}
						}
					if(objIter->counter) objIter->counter--;	// decrement counter
					if(!objIter->counter)
						{
						objIter->behaviour = 0;	// switch off if finished count
						objIter->SetPosition(0.0f, 1000.0f, 0.0f);
						}
					}
				}
			else if(4 == objIter->modelNumber)			// GREEN LASER
				{
				if(1 == objIter->behaviour)
					{
					// not used yet
					}
				}
			}

		// 3. Fragment behaviour
		objList = Scene.GetObjectList(FRAG_LIST);
		for(objIter = objList->begin(); objIter != objList->end(); objIter++)
			{
			if(5 == objIter->modelNumber)			// FRAG
				{
				if(1 == objIter->behaviour)
					{
					objIter->position += objIter->velocity;
					if(objIter->position.y < 0.0)
						objIter->velocity.y *= -1.0f;			// bounce off floor
					objIter->velocity.y -= 0.05f;				// gravity
					objIter->rotation.x += objIter->velocity.x * 0.1f;
					objIter->rotation.y += objIter->velocity.y * 0.1f;
					objIter->rotation.z += objIter->velocity.z * 0.1f;
					if(objIter->counter) objIter->counter--;	// decrement counter
					if(!objIter->counter)
						{
						objIter->behaviour = 0;	// switch off if finished count
						objIter->SetPosition(0.0f, 1000.0f, 0.0f);
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

		if(keys[SDLK_LEFT] == SDL_PRESSED )
			heading += 0.03f;
		else if(keys[SDLK_RIGHT] == SDL_PRESSED )
			heading -= 0.03f;

		Vector dirn((float)sin(heading), 0.0f, (float)cos(heading));
		camera->SetDirection(dirn);

		Vector posn = camera->GetPosition();
		if(keys[SDLK_UP] == SDL_PRESSED ) {
			posn.x += dirn.x * 0.5f;
			posn.z += dirn.z * 0.5f;
			camera->SetPosition(posn);
		}
		else if(keys[SDLK_DOWN] == SDL_PRESSED ) {
			posn.x -= dirn.x * 0.5f;
			posn.z -= dirn.z * 0.5f;
			camera->SetPosition(posn);
		}

		if(keys[SDLK_SPACE] == SDL_PRESSED ) {
			if(0 == debounce) {
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
						bulletIter->SetPosition(posn.x, 2.0f, posn.z);
						bulletIter->SetVelocity(dirn.x * 1.0f, 0.0f, dirn.z * 1.0f);
						bulletIter->rotation.y = heading;
						break;
						}
					}
				debounce = 12;
				}
			}

		if(debounce) debounce--;
		} // wend

    printf("Quiting SDL.\n");
    
    // Shutdown all subsystems 
    SDL_Quit();
    
    exit(0);
}

