/*
 *	E3D example 5: Battlezone
 *	James Higgs 2006/2007
 *
 *	Based on PSPDEV example code
 */

// TODO: playfield extents
// TODO: sights and score overlay

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <pspctrl.h>

#include <pspgu.h>
#include <pspgum.h>

extern "C" {
//#include "../../../../sdk/samples/gu/common/callbacks.h"
//#include "../../../../sdk/samples/gu/common/vram.h"
#include "callbacks.h"
#include "vram.h"
}

#include "../../../include/E3D.h"				// includes all the E3D classes

// Set our C++ namespace prefix to E3D so we can more easily use E3D objects
using namespace E3D;

#define printf pspDebugScreenPrintf

PSP_MODULE_INFO("E3D Ex. 5 - Battlezone", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)

#define OBJ_MAX 400

// our E3D scene
class E3D_Scene Scene;

static unsigned int __attribute__((aligned(16))) list[262144];

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
int main(int argc, char* argv[]) {
	int i, j;
	int		frames;
	E3D_Object *pObject;
	SceCtrlData pad;

	// Set up PSP 
	pspDebugScreenInit();
	setupCallbacks();

	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL); //PSP_CTRL_MODE_ANALOG);

	pspDebugScreenSetXY(0, 2);

	printf("Initialising E3D scene... \n");

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

	printf("Loading models... \n");

	Scene.AddModel("block", "block.obj");
	Scene.AddModel("ufo", "ufo.obj");
	Scene.AddModel("tank", "tank.obj");
	Scene.AddModel("laser_yellow", "laser_yellow.obj");
	Scene.AddModel("laser_green", "laser_green.obj");
	Scene.AddModel("frag", "frag.obj");

	printf("Models loaded. \n");

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

	printf("Objects added. \n");

	// setup GU
	void* fbp0 = getStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888);
	void* fbp1 = getStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888);
	void* zbp = getStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_4444);

	sceGuInit();

	sceGuStart(GU_DIRECT,list);
	sceGuDrawBuffer(GU_PSM_8888,fbp0,BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,fbp1,BUF_WIDTH);
	sceGuDepthBuffer(zbp,BUF_WIDTH);
	sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
	sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
	sceGuDepthRange(65535,0);
	sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CCW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_CULL_FACE);
	//sceGuEnable(GU_TEXTURE_2D);
	sceGuEnable(GU_CLIP_PLANES);
	sceGuEnable(GU_LIGHTING);
	sceGuEnable(GU_LIGHT0);
	sceGuFinish();
	sceGuSync(0,0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);

	//Scene.GetBackColour();
	sceGuClearColor(0xff000000);

	// make sure the VIEW matrix is initialised
	sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();

    // Main loop
	float heading = 0.0;
    frames = 0;
	int debounce = 0;

    while( running() ) {
        // Get analog joystick position
		sceCtrlReadBufferPositive(&pad, 1); 
        frames ++;

		sceGuStart(GU_DIRECT,list);

		// clear screen
		sceGuClearDepth(0);
		sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);

		// animate
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

		// Tell E3D to render it's scene to the current GU context
		Scene.Render((float)SCR_WIDTH, (float)SCR_HEIGHT, true);

		sceGuFinish();
		sceGuSync(0,0);

		sceDisplayWaitVblankStart();
		sceGuSwapBuffers();

		if(pad.Buttons & PSP_CTRL_LEFT)
			heading += 0.03f;
		else if(pad.Buttons & PSP_CTRL_RIGHT)
			heading -= 0.03f;

		Vector dirn((float)sin(heading), 0.0f, (float)cos(heading));
		camera->SetDirection(dirn);

		Vector posn = camera->GetPosition();
		if(pad.Buttons & PSP_CTRL_UP) {
			posn.x += dirn.x * 0.5f;
			posn.z += dirn.z * 0.5f;
			camera->SetPosition(posn);
		}
		else if(pad.Buttons & PSP_CTRL_DOWN) {
			posn.x -= dirn.x * 0.5f;
			posn.z -= dirn.z * 0.5f;
			camera->SetPosition(posn);
		}

		if(pad.Buttons & PSP_CTRL_CROSS) {
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

    // Exit back to PSP OS
	sceGuTerm();
	sceKernelExitGame();
	return 0;									// Exit The Program
}

