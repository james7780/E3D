/*
 *		E3D example 5: Battlezone
 *		James Higgs 2006
 *
 *		This Code based on Jeff Molofee's OpenGL template
 */

// TODO: playfield extents
// TODO: sights and score overlay

#include <stdio.h>
#include <assert.h>
#include "bcm_host.h"
#include "GLES/gl.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include <SDL/SDL.h>					// For input only


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



#define OBJ_MAX 100		//400

class E3D_Scene Scene;
//class E3D_Model gpModel[OBJ_MAX];
//class E3D_Object gpObject[OBJ_MAX];

//E3D_Utility util;

// object list defines
enum {
	PLAYER_LIST	= 0,
	BULLET_LIST = 1,
	ENEMY_LIST	= 2,
	FRAG_LIST	= 3
};


// Display state stuff
typedef struct
{
	uint32_t screenWidth;
	uint32_t screenHeight;
	// OpenGL|ES objects
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	GLuint tex;
	// current distance from camera
	GLfloat distance;
} GLES_STATE_T;

static GLES_STATE_T glesState;

/// Setup display and OpenGL ES stuff
int InitGL(GLES_STATE_T *state)										// All Setup For OpenGL Goes Here
{
	printf("InitGL()\n");

  int32_t success = 0;
   EGLBoolean result;
   EGLint num_config;

   static EGL_DISPMANX_WINDOW_T nativewindow;

   DISPMANX_ELEMENT_HANDLE_T dispman_element;
   DISPMANX_DISPLAY_HANDLE_T dispman_display;
   DISPMANX_UPDATE_HANDLE_T dispman_update;
   VC_RECT_T dst_rect;
   VC_RECT_T src_rect;

   static const EGLint attribute_list[] =
   {
      EGL_RED_SIZE, 8,
      EGL_GREEN_SIZE, 8,
      EGL_BLUE_SIZE, 8,
      EGL_ALPHA_SIZE, 8,
      //EGL_DEPTH_SIZE, 16,
      //EGL_SAMPLES, 4,
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      EGL_NONE
   };
   
   EGLConfig config;

   // get an EGL display connection
   state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
   assert(state->display != EGL_NO_DISPLAY);

   // initialize the EGL display connection
   result = eglInitialize(state->display, NULL, NULL);
   assert(EGL_FALSE != result);

   // get an appropriate EGL frame buffer configuration
   // this uses a BRCM extension that gets the closest match, rather than standard which returns anything that matches
   //result = eglSaneChooseConfigBRCM(state->display, attribute_list, &config, 1, &num_config);
   // use standard version
   result = eglChooseConfig(state->display, attribute_list, &config, 1, &num_config);
    assert(EGL_FALSE != result);

   // create an EGL rendering context
   state->context = eglCreateContext(state->display, config, EGL_NO_CONTEXT, NULL);
   assert(state->context!=EGL_NO_CONTEXT);

   // create an EGL window surface
   success = graphics_get_display_size(0 /* LCD */, &state->screenWidth, &state->screenHeight);
   assert( success >= 0 );

   dst_rect.x = 0;
   dst_rect.y = 0;
   dst_rect.width = state->screenWidth;
   dst_rect.height = state->screenHeight;
/*
   // JH - testing
   dst_rect.x = state->screenWidth / 2;
   dst_rect.y = state->screenHeight / 2;
   dst_rect.width = state->screenWidth / 2;
   dst_rect.height = state->screenHeight / 2;
*/
   src_rect.x = 0;
   src_rect.y = 0;
   src_rect.width = state->screenWidth << 16;
   src_rect.height = state->screenHeight << 16;        

   dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
   dispman_update = vc_dispmanx_update_start( 0 );

   DISPMANX_TRANSFORM_T xform = DISPMANX_NO_ROTATE;
   dispman_element = vc_dispmanx_element_add ( dispman_update, dispman_display,
      0/*layer*/, &dst_rect, 0/*src*/,
      &src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, xform/*transform*/);
      
   nativewindow.element = dispman_element;
   nativewindow.width = state->screenWidth;
   nativewindow.height = state->screenHeight;
   vc_dispmanx_update_submit_sync( dispman_update );
      
   state->surface = eglCreateWindowSurface( state->display, config, &nativewindow, NULL );
   assert(state->surface != EGL_NO_SURFACE);

   // connect the context to the surface
   result = eglMakeCurrent(state->display, state->surface, state->surface, state->context);
   assert(EGL_FALSE != result);

   // Enable back face culling.
   glEnable(GL_CULL_FACE);

   glMatrixMode(GL_MODELVIEW);
/*
   glEnable(GL_DEPTH_TEST);
   glClearDepthf(1.0);
   glDepthFunc(GL_LEQUAL);

   float noAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
   glLightfv(GL_LIGHT0, GL_AMBIENT, noAmbient);
   glEnable(GL_LIGHT0);
   glEnable(GL_LIGHTING);
*/
	return TRUE;										// Initialization Went OK
}

// Function to be passed to atexit().
static void exit_func(void)
{
	printf("\nexit_func()\n");

/*
   if (eglImage != 0)
   {
      if (!eglDestroyImageKHR(glesState.display, (EGLImageKHR) eglImage))
         printf("eglDestroyImageKHR failed.");
   }
*/

   // clear screen
   glClear(GL_COLOR_BUFFER_BIT);
   eglSwapBuffers(glesState.display, glesState.surface);

   // Release OpenGL resources
   eglMakeCurrent( glesState.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
   eglDestroySurface( glesState.display, glesState.surface );
   eglDestroyContext( glesState.display, glesState.context );
   eglTerminate( glesState.display );

} // exit_func()

int DrawGLScene(GLES_STATE_T *state)									// Here's Where We Do All The Drawing
{
	// Tell E3D to render it's scene to the current GL context
	//Scene.Render(VIEW_WIDTH, VIEW_HEIGHT, true);
	Scene.Render(state->screenWidth, state->screenHeight, true);
	eglSwapBuffers(state->display, state->surface);

	return TRUE;						// Jump Back
}

/*
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
*/

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

	printf("Running...\n");
	getchar();

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

    // Enable debug output
    E3D_debug = 1;

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

	printf("Loading models\n");
	getchar();

	Scene.AddModel("block", "block.obj");
	Scene.AddModel("ufo", "ufo.obj");
	Scene.AddModel("tank", "tank.obj");
	Scene.AddModel("laser_yellow", "laser_yellow.obj");
	Scene.AddModel("laser_green", "laser_green.obj");
	Scene.AddModel("frag", "frag.obj");

	printf("Adding objects\n");
getchar();
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

 	bcm_host_init();
	printf("Note: ensure you have sufficient gpu_mem configured\n");
	atexit(exit_func);

	// Clear application state
	memset(&glesState, 0, sizeof(glesState) );

	// Set up OpenGL ES 
    InitGL(&glesState);

	// Init GL with no window
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
		printf("Unable to init SDL: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
		}

	SDL_Surface *sdlSurface = SDL_SetVideoMode(0, 0, 32, SDL_SWSURFACE);
	
    //SDL_WM_SetCaption("E3D Example 5 - Battlezone", "");

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

        frames++;

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

		DrawGLScene(&glesState);
/*
	    // Swap the double-buffer
	    SDL_GL_SwapBuffers( );
*/
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
		
	// Shutdown all subsystems 
    printf("Quitting....\n");
	//exit_func();
	SDL_Quit();
    exit(0);
}

