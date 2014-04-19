/*
 * E3D Example 4 - Tailgunner
 * James Higgs (Jum) 2006
 *
 * Based on PSPDEV example code
 */

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

PSP_MODULE_INFO("E3D Test", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

// our E3D scene
class E3D_Scene Scene;

static unsigned int __attribute__((aligned(16))) list[262144];

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)

// add some fragments to the scene
void addFrags(Vector p, Vector v) {
	int i, j;
	// add 10 fragments
	for(i=0; i<10; i++) {
		for(j=10; j<=39; j++) {
			E3D_Object *pObject = Scene.GetObject(j);
			if(0 == pObject->behaviour) {
				pObject->behaviour = 1;
				pObject->counter = 50;
				pObject->position.x = p.x;
				pObject->position.y = p.y;
				pObject->position.z = p.z;
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

int main(int argc, char* argv[])
{
	int i, j;
	E3D_Object *pObject;
	SceCtrlData pad;

	pspDebugScreenInit();
	setupCallbacks();

	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

	pspDebugScreenSetXY(0, 2);

	// set up the scene
	Scene.Start();
	Scene.SetRenderDistance(400.0f);

	// Setup our scene's default light
	E3D_Light *light0 = Scene.GetLight(0);
	light0->SetColour( 0.8f, 0.8f, 0.6f );
	light0->SetAmbient( 0.4f, 0.4f, 0.4f );
	light0->SetDirection( 0.707f, 0.707f, 0.0f );

	// Setup camera with a default view
	E3D_Camera *camera = Scene.AddCamera();
	camera->SetPosition(0.0f, 0.0f, 0.0f);
	camera->SetDirection(0.0f, 0.0f, 1.0f);
	camera->SetUpVector(0.0f, 1.0f, 0.0f);
	camera->SetFOV(60.0f);

	printf("Loading models... \n");

	Scene.AddModel("sights", "sights.obj");
	Scene.AddModel("glider", "glider.obj");
	Scene.AddModel("frag", "frag.obj");
	Scene.AddModel("laser_yellow", "laser_yellow.obj");
	Scene.AddModel("laser_green", "laser_green.obj");

	printf("Models loaded. \n");

	// add sights
	pObject = Scene.AddObject("sights");
	pObject->SetPosition(0.0f, 0.0f, 10.0f);

	// add some bullets
	for(i=1; i<9; i++) {
		pObject = Scene.AddObject("laser_yellow");
		pObject->SetPosition(50.0f, 0.0f, 0.0f);				// offscreen
	}

	// add some fragments (objects 10 to 50)
	for(i=10; i<40; i++) {
		pObject = Scene.AddObject("frag");
		pObject->SetPosition(50.0f, 0.0f, 0.0f);				// offscreen
	}

	// manufacture some gliders
	for(i=40; i<100; i++) {
		pObject = Scene.AddObject("glider");
		pObject->SetPosition(rand() % 60 - 30, rand() % 40 - 20, 50.0f + rand() % 300);
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

	int val = 0;
	int debounce = 0;

	//float dx = 0.5f;
	//float dy = 0.2f;
	float dz = 0.0f;

	unsigned char joyx, joyy;

	while(running())
	{
		sceCtrlReadBufferPositive(&pad, 1); 
/*
		printf("Analog X = %d ", pad.Lx);
		printf("Analog Y = %d \n", pad.Ly);
*/
		joyx = pad.Lx;
		joyy = pad.Ly;

		sceGuStart(GU_DIRECT,list);

		// clear screen
		sceGuClearDepth(0);
		sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);

		// animate
		dz += 0.05f;
		pObject = Scene.GetObject(0);
		pObject->rotation.z = dz;
		pObject->position.x = (float)(128-joyx) / 20.0f;
		pObject->position.y = (float)(128-joyy) / 20.0f;

		for(i=1; i<Scene.GetNumObjects(); i++) {
			pObject = Scene.GetObject(i);
			switch(pObject->GetModelNumber()) {
				case 1 : // this is a glider
					if(1 == pObject->behaviour) {
						pObject->SetRotation(0.0f, DEG2RAD(180), 0.0f);
						pObject->position.z -= 1.0f;
						if(pObject->position.z < 0.0f) {
							pObject->position.z = 300.0f + rand() % 50;
						}
					}
					break;
				case 2 : // fragment
					if(1 == pObject->behaviour) {
						pObject->position += pObject->velocity;
						pObject->rotation += (pObject->velocity * 0.1f);
						if(pObject->counter) pObject->counter--;	// decrement counter
						if(!pObject->counter) {
							pObject->behaviour = 0;	// switch off if finished count
							pObject->position.x = 50.0f;
							pObject->position.z = 0.0f;
						}
					}
					break;
				case 3 : // bullet
					if(1 == pObject->behaviour)	{	// active
						pObject->position += pObject->velocity;
						// check if this bullet has hit any gliders
						for(j=40; j<Scene.GetNumObjects(); j++) {
							E3D_Object *pGlider = Scene.GetObject(j);
							if(fabs(pGlider->position.z - pObject->position.z) < 5.0f) {
								if(fabs(pGlider->position.x - pObject->position.x) < 5.0f) {
									if(fabs(pGlider->position.y - pObject->position.y) < 5.0f) {
										// hit!
										Vector v(0.0f, 0.0f, -1.0f);
										addFrags(pGlider->position, v);
										pGlider->position.z = 0.0f;
										pGlider->position.x = 50.0f;
										pGlider->behaviour = 0;		// switch off glider
										pObject->counter = 1;		// expire bullet
									}
								}
								
							}
						}
						if(pObject->counter) pObject->counter--;	// decrement counter
						if(!pObject->counter) {
							pObject->behaviour = 0;	// switch off if finished count
							pObject->position.z = 0.0f;
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

		// get Mouse Button
		if(pad.Buttons & PSP_CTRL_CROSS) {
			if(0 == debounce) {
				// find first free bullet and launch it
				for(i=1; i<7; i++) {
					pObject = Scene.GetObject(i);
					if(0 == pObject->behaviour) {
						pObject->behaviour = 1;
						pObject->counter = 100;
						pObject->position.x = -1.0f;
						pObject->position.y = -1.0f;
						pObject->position.z = 0.0f;
						pObject->velocity.x = (float)((SCR_WIDTH/2) - joyx) / 200.0f;
						pObject->velocity.y = (float)((SCR_HEIGHT/2) - joyy) / 200.0f;
						pObject->velocity.z = 2.0f;
						pObject->rotation.y = (float)((SCR_WIDTH/2) - joyx) / 400.0f;
						pObject->rotation.x = (float)((SCR_HEIGHT/2) - joyy) / -400.0f;
						i = 8;
					}
				}
				debounce = 12;
			}
		}

		if(debounce) debounce--;

		val++;
	}

	sceGuTerm();

	sceKernelExitGame();
	return 0;
}
