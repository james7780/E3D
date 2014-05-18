/*
 *		E3D Example 1 - Scene with 1 object
 *		James Higgs 2006
 *
 */

//#include "SDL.h"   /* All SDL App's need this */
#include <stdio.h>
#include <assert.h>
//#include <GL/gl.h>
//#include <GL/glu.h>
#include "bcm_host.h"
#include "GLES/gl.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include <vector>

// NB: Include our E3D header file
#include "../../../include/E3D.h"
// NB: Set our C++ namespace prefix to E3D so we can more easily use E3D objects
using namespace E3D;

#define VIEW_WIDTH		600
#define VIEW_HEIGHT		400

GLfloat zoom = 20;

class E3D_Scene Scene;

typedef struct
{
   uint32_t screenWidth;
   uint32_t screenHeight;
// OpenGL|ES objects
   EGLDisplay display;
   EGLSurface surface;
   EGLContext context;
   GLuint tex;
// model rotation vector and direction
//   GLfloat rot_angle_x_inc;
//   GLfloat rot_angle_y_inc;
//   GLfloat rot_angle_z_inc;
// current model rotation angles
//   GLfloat rot_angle_x;
//   GLfloat rot_angle_y;
//   GLfloat rot_angle_z;
// current distance from camera
   GLfloat distance;
   GLfloat distance_inc;
//   MODEL_T model;
} GLES_STATE_T;

static GLES_STATE_T glesState;

/// Setup display and OpenGL ES stuff
int InitGL(GLES_STATE_T *state)
{
	printf("InitGL()\n");
	
/* Old GL stuff (for reference)
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
*/

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

   // Set background color and clear buffers
   glClearColor(0.15f, 0.25f, 0.35f, 1.0f);
   //glClearColor(0.5f, 0.3f, 0.1f, 0.5f);
  // glClearColor(1.0f, 0.3f, 0.1f, 1.0f);

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

/***********************************************************
 * Name: init_model_proj
 *
 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description: Sets the OpenGL|ES model to default values
 *
 * Returns: void
 *
 ***********************************************************/
static void InitProjection(GLES_STATE_T *state)
{
	printf("InitProjection()\n");
	
   float nearp = 0.1f;
   float farp = 500.0f;
   float hht;
   float hwd;

   glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

   glViewport(0, 0, (GLsizei)state->screenWidth, (GLsizei)state->screenHeight);
      
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   hht = nearp * (float)tan(45.0 / 2.0 / 180.0 * M_PI);
   hwd = hht * (float)state->screenWidth / (float)state->screenHeight;

   glFrustumf(-hwd, hwd, -hht, hht, nearp, farp);
   
   glEnableClientState( GL_VERTEX_ARRAY );
	//glVertexPointer( 3, GL_BYTE, 0, quadx );
   

	// TODO
   //reset_model(state);
   // reset model position
   // reset model position
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef(0.f, 0.f, -50.f);

	state->distance = 40.0f;
}

// TEST
/***********************************************************
 * Name: redraw_scene
 *
 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description:   Draws the model and calls eglSwapBuffers
 *                to render to screen
 *
 * Returns: void
 *
 ***********************************************************/
static void DrawGLScene(GLES_STATE_T *state)
{
 //  // Start with a clear screen
 //  glClearColor(1.0f, 0.3f, 0.1f, 1.0f);
 //  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

/*
   // Draw first (front) face:
   // Bind texture surface to current vertices
   glBindTexture(GL_TEXTURE_2D, state->tex[0]);

   // Need to rotate textures - do this by rotating each cube face
   glRotatef(270.f, 0.f, 0.f, 1.f ); // front face normal along z axis

   // draw first 4 vertices
   glDrawArrays( GL_TRIANGLE_STRIP, 0, 4);

   // same pattern for other 5 faces - rotation chosen to make image orientation 'nice'
   glBindTexture(GL_TEXTURE_2D, state->tex[1]);
   glRotatef(90.f, 0.f, 0.f, 1.f ); // back face normal along z axis
   glDrawArrays( GL_TRIANGLE_STRIP, 4, 4);

   glBindTexture(GL_TEXTURE_2D, state->tex[2]);
   glRotatef(90.f, 1.f, 0.f, 0.f ); // left face normal along x axis
   glDrawArrays( GL_TRIANGLE_STRIP, 8, 4);

   glBindTexture(GL_TEXTURE_2D, state->tex[3]);
   glRotatef(90.f, 1.f, 0.f, 0.f ); // right face normal along x axis
   glDrawArrays( GL_TRIANGLE_STRIP, 12, 4);

   glBindTexture(GL_TEXTURE_2D, state->tex[4]);
   glRotatef(270.f, 0.f, 1.f, 0.f ); // top face normal along y axis
   glDrawArrays( GL_TRIANGLE_STRIP, 16, 4);

   glBindTexture(GL_TEXTURE_2D, state->tex[5]);
   glRotatef(90.f, 0.f, 1.f, 0.f ); // bottom face normal along y axis
   glDrawArrays( GL_TRIANGLE_STRIP, 20, 4);
*/

	// Draw our E3D scene
	Scene.Render(state->screenWidth, state->screenHeight, true);
	eglSwapBuffers(state->display, state->surface);
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
   glClear( GL_COLOR_BUFFER_BIT );
   eglSwapBuffers(glesState.display, glesState.surface);

   // Release OpenGL resources
   eglMakeCurrent( glesState.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
   eglDestroySurface( glesState.display, glesState.surface );
   eglDestroyContext( glesState.display, glesState.context );
   eglTerminate( glesState.display );

} // exit_func()

/////////////////////////////////////////////////////////////////////////////////
// Main loop
/////////////////////////////////////////////////////////////////////////////////
int main(void) {

	bool done = false;								// Bool Variable To Exit Loop
	int debounce = 0;

	int		running, frames, x, y;
    double  t, t0, fps;
    char    titlestr[ 200 ];
    
    // Enable debug output
    E3D_debug = 1;
    
	E3D_Object *pObject;

	// Set up our scene
	Scene.Start();
	Scene.SetBackColour(0.0f, 0.5f, 0.0f);		// dark green
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
	E3D_Model *model = Scene.AddModel("flipper", "flipper.obj");
	model->CalculateNormals();

	// Create a 3D object instance in the scene
	pObject = Scene.AddObject(0, "flipper");
	pObject->SetPosition(0.0f, 0.0f, -50.0f);

	// make our camera look at the object
	camera->LookAt(pObject->position);

/* For reference
    // Set up SDL video buffer
    printf("Initializing SDL.\n");
 
    // Initialize defaults, Video and Audio 
    if((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO)==-1)) { 
        printf("Could not initialize SDL: %s.\n", SDL_GetError());
        exit(-1);
    }

    printf("SDL initialized.\n");

    // set quit cleanup callback
    atexit(SDL_Quit);
*/

	bcm_host_init();
	printf("Note: ensure you have sufficient gpu_mem configured\n");

	// Clear application state
	memset(&glesState, 0, sizeof(glesState) );

     /*
     * At this point, we should have a properly setup
     * double-buffered window for use with OpenGL.
     */
    InitGL(&glesState);
    
	// Setup the model world projection
	InitProjection(&glesState);
/*
	// Initialize SDL input
    if(-1 == SDL_Init(SDL_INIT_TIMER | SDL_INIT_JOYSTICK))
		{ 
        printf("Could not initialize SDL: %s.\n", SDL_GetError());
        exit(-1);
		}

    printf("SDL initialized.\n");
*/

	// Test
	//glEnable(GL_TEXTURE_2D);

	// update model view projection
	glLoadIdentity();
	// move camera back to see the cube
	glTranslatef(0.f, 0.f, -glesState.distance);

	glLineWidth(5.0f);

	DrawGLScene(&glesState);
		
	printf("Press space to continue...");
getchar();

    // Main loop
    running = GL_TRUE;
    frames = 0;
//	unsigned int ticks = SDL_GetTicks();
    for (int i = 0; i < 100; i++) {
/*
		// limit fps to 50 frames per second
		while((SDL_GetTicks() - ticks) < 20) ;
		ticks = SDL_GetTicks();

		//pObject->rotation.y += 0.05f;
		//pObject->rotation.x += 0.03f;

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

        //
       DrawGLScene();

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
*/
		pObject->SetPosition(0.0f, 0.0f, -i);

		
		
		DrawGLScene(&glesState);
	} // wend

//    printf("Quitting SDL.\n");
    
    // Shutdown all subsystems 
//    SDL_Quit();
    
    printf("Quitting....\n");
	
	exit_func();

    exit(0);
}

