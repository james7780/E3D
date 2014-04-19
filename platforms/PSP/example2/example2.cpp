/*
 * E3D Example 2 - Load a scene file
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

	// initialise and load our scene file
	Scene.Start();
	Scene.SetRenderDistance(400.0f);
	Scene.Load("test.scn");

	// get the first light in the scene
	E3D_Light *light1 = Scene.GetLight(0);
	if(!light1) {
		light1 = Scene.AddLight();
		light1->SetColour( 0.8f, 0.8f, 0.6f );
		light1->SetDirection( 0.0f, 0.7f, 0.7f );
	}

	// get the first camera in the scene
	E3D_Camera *camera = Scene.GetCamera(0);
	if(!camera) {
		camera = Scene.AddCamera();
		camera->SetFOV(60.0f);
	}

	//rgb_f bgCol = Scene.GetBackColour();
	sceGuClearColor(0xff554433);

	// make sure the VIEW matrix is initialised
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

		// look at the centre of the 3D scene
		float zoom = 50;
		ScePspFVector3 eye;
		eye.x = (float)(sin(bearing) * zoom);
		eye.y = 10.0;
		eye.z = (float)(-cos(bearing) * zoom);
		camera->SetPosition(eye.x, eye.y, eye.z);
		camera->LookAt(0.0f, 1.0f, 0.0f);

		bearing += 0.003;


		// Tell E3D to render it's scene to the current GU context
		Scene.Render((float)SCR_WIDTH, (float)SCR_HEIGHT, true);

		sceGuFinish();
		sceGuSync(0,0);

		sceDisplayWaitVblankStart();
		sceGuSwapBuffers();

		val++;
	}

	sceGuTerm();

	sceKernelExitGame();
	return 0;
}
