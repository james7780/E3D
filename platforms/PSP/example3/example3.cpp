/*
 * E3D Example 3 - Create an animated scene
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

int main(int argc, char* argv[])
{
	setupCallbacks();

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

	// Reset the scene
	Scene.Start();
	Scene.SetRenderDistance(400.0f);

	// Setup our scene's default light
	E3D_Light *light0 = Scene.GetLight(0);
	light0->SetColour( 1.0f, 1.0f, 1.0f );
	light0->SetDirection( 0.707f, 0.707f, 0.0f );

	// Setup camera with a default view
	E3D_Camera *camera = Scene.GetCamera(0);
	camera->SetPosition(0.0f, 10.0f, -20.0f);
	camera->SetDirection(0.0f, 0.0f, 1.0f);
	camera->SetUpVector(0.0f, 1.0f, 0.0f);
	camera->SetFOV(60.0f);

	// Add the required models to the scene
	Scene.AddModel("carbody", "stcar.3do");
	Scene.AddModel("wheel", "wheel2.3do");
	Scene.AddModel("tree", "tree.3do");

	// add a car to the scene, then add some wheels to the car
	E3D_Object *pObject = Scene.AddObject("carbody");
	pObject->SetPosition(0.0f, 0.0f, -300.0f);
	pObject = Scene.AddObject("wheel");		// front left
	pObject->position.x = 1.0f;
	pObject->position.z = 1.2f;
	Scene.AttachObject(1, 0);
	pObject = Scene.AddObject("wheel");		// front right
	pObject->position.x = -1.0f;
	pObject->position.z = 1.2f;
	Scene.AttachObject(2, 0);
	pObject = Scene.AddObject("wheel");		// rear left
	pObject->position.x = 1.0f;
	pObject->position.z = -1.0f;
	Scene.AttachObject(3, 0);
	pObject = Scene.AddObject("wheel");		// rear right
	pObject->position.x = -1.0f;
	pObject->position.z = -1.0f;
	Scene.AttachObject(4, 0);

	// finally, add a few trees to the scene
	for(int i=0; i<30; i++) {
		pObject = Scene.AddObject("tree");		// trees
		pObject->position.x = (float)(rand()%400 - 200);
		pObject->position.z = (float)(rand()%400 - 200);
		pObject->SetScale(2.0, 2.0, 2.0);
	}

	//Scene.GetBackColour();
	sceGuClearColor(0xff554433);

	// make sure VIEW matrix is initialised
	sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();

	int val = 0;
	float bearing = 0.0;

	while(running())
	{
		sceGuStart(GU_DIRECT,list);

		// clear screen
		sceGuClearDepth(0);
		sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);

		// look at the object in the 3D scene
		E3D_Object *pTargetObject = Scene.GetObject(0);
		float zoom = 10.0;
		ScePspFVector3 eye;
		eye.x = pTargetObject->position.x + (float)(sin(bearing) * zoom);
		eye.y = 3.0;
		eye.z = pTargetObject->position.z + (float)(-cos(bearing) * zoom);
		camera->SetPosition(eye.x, eye.y, eye.z);
		camera->LookAt(pTargetObject->position);

		// animate scene
		bearing += 0.01f;
		// move car
		Scene.GetObject(0)->position.z += 0.4f;
		// rotate wheels
		for(int i=1; i < 5; i++) {
			Scene.GetObject(i)->rotation.x += 6.0f;
		}

		// Tell E3D to render it's scene to the current GU context
		Scene.Render((float)SCR_WIDTH, (float)SCR_HEIGHT, true);

		sceGuFinish();
		sceGuSync(0,0);

		sceDisplayWaitVblankStart();
		sceGuSwapBuffers();

		printf("Blarg!\n");

		val++;
	}

	sceGuTerm();

	sceKernelExitGame();
	return 0;
}
