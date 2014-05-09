/*
 *		E3D example 6: WebFest (tempest clone)
 *		James Higgs 2006/2007
 *
 *		Based on PSPDEV example code
 */

// Changlelog:'
// Changlelog:
// 31/03/2008: 1. Rebalanced enemy spawning timing (ie: enemy start distances)
//
// 24/03/2008: 1. If killed while going down tube, now go to next level.
//             2. Pressing start key (fire) in title screen is disabled for 50 frames (avoids immediate restart after hi-score screen)
//             3. Fixed UFO behaviour
//             4. Adjusted spawning of enemies to avoid "bunching" in later levels (makes them more playable).
//             5. Player takes energy hit if spike reaches top
//
// 25/12/2007: 1. Fixed webs 17 to 20 (were all flat before)
//             2. Added enemy spawn data for levels 17 to 20
//             3. Player ship can take 1 hit if charge meter is full
//             4. Added camera shake if player is hit
//             5. Added "BARRIER" and "UFO" enemies

#define DO_STARS
#define DEBUG_START_LEVEL	0
#define SPAWNPERIOD			800

// Ideas:
// 1. "Timeslip"
//    - like "sand of time"
//    - slows down time
//    - useful if getting overrun
//    - "slip-meter" fills up as you kill beasties (or collect powerup)
//    - can only use if meter if full
//    - can also use full slip-meter for "bulling" or "shockwave" or "bezerk"
// 2. "Bezerk"
//    - uses power meter
//    - when activated, player ship scans across playfield from one end to the other and back again,
//      firing a shot in each segment
// 
// TODO:
// 1. Tuning of levels (difficulty curve)

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <pspctrl.h>
#include <pspaudio.h>
#include <psppower.h>

#include <pspgu.h>
#include <pspgum.h>

extern "C" {
//#include "../../../../sdk/samples/gu/common/callbacks.h"
//#include "../../../../sdk/samples/gu/common/vram.h"
#include "callbacks.h"
#include "vram.h"
}

#include "hiscore.h"				// generic high-score module

// NB: Include our E3D header file
#include "../../../include/E3D.h"
// NB: Set our C++ namespace prefix to E3D so we can more easily use E3D objects
using namespace E3D;

#define printf pspDebugScreenPrintf

PSP_MODULE_INFO("WebFest 002", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

#include "webfest.h"
#include "vecglyph.h"

#define WHITE	0xFFFFFFFF;
#define BLUE	0xFFFF0000;
#define RED		0xFF0000FF;
#define GREEN	0xFF00FF00;
#define YELLOW	0xFF00FFFF;
#define PURPLE	0xFFFF00FF;
#define CYAN	0xFFFFFF00;
#define LGREY	0xFFC0C0C0;
#define MGREY	0xFF808080;
#define DGREY	0xFF404040;

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)

#define VIEW_WIDTH	SCR_WIDTH
#define VIEW_HEIGHT	SCR_HEIGHT

#define GLYPH_WIDTH		12
#define GLYPH_HEIGHT	12

#define STARFIELD_WIDTH	600

// global game variables
unsigned int g_lives = 3;
unsigned int g_score = 0;
unsigned int g_nextBonusScore = 0;
int g_charge = 0;			// charge meter
unsigned int g_shotsFired = 0;
unsigned int g_shotsHit = 0;
unsigned int g_shakeCount = 0;

#define OBJ_MAX			400
#define RELOAD_COUNT	12
#define OFFSCREEN		0.0f, 5000.0f, 0.0f

// Our scene!
E3D_Scene Scene;

// object instance map for player object list
// NB: Objects must be added to the player object list in this order
#define TUBE_ID			0
#define PLAYER1_ID		1
#define PLAYER2_ID		2

#include "enemy_seq.h"

// samples
#include "audio/samples.h"

E3D::Colour webColour[20] = {
	E3D::Colour( 0.0f, 0.0f, 1.0f ),	// web 00 - blue
	E3D::Colour( 0.0f, 0.0f, 1.0f ),	// web 01 - blue
	E3D::Colour( 0.0f, 0.0f, 1.0f ),	// web 02 - blue
	E3D::Colour( 0.0f, 0.0f, 1.0f ),	// web 03 - blue
	E3D::Colour( 0.0f, 1.0f, 0.0f ),	// web 04 - green
	E3D::Colour( 0.0f, 1.0f, 0.0f ),	// web 05 - green
	E3D::Colour( 0.0f, 1.0f, 0.0f ),	// web 06 - green
	E3D::Colour( 0.0f, 1.0f, 0.0f ),	// web 07 - green
	E3D::Colour( 1.0f, 0.5f, 0.0f ),	// web 08 - orange
	E3D::Colour( 1.0f, 0.5f, 0.0f ),	// web 09 - orange
	E3D::Colour( 1.0f, 0.5f, 0.0f ),	// web 10 - orange
	E3D::Colour( 1.0f, 0.5f, 0.0f ),	// web 11 - orange
	E3D::Colour( 0.0f, 1.0f, 1.0f ),	// web 12 - cyan
	E3D::Colour( 0.0f, 1.0f, 1.0f ),	// web 13 - cyan
	E3D::Colour( 0.0f, 1.0f, 1.0f ),	// web 14 - cyan
	E3D::Colour( 0.0f, 1.0f, 1.0f ),	// web 15 - cyan
	E3D::Colour( 1.0f, 0.5f, 0.5f ),	// web 16 - pink
	E3D::Colour( 0.5f, 1.0f, 0.5f ),	// web 17 - lime
	E3D::Colour( 1.0f, 1.0f, 1.0f ),	// web 18 - white
	E3D::Colour( 0.2f, 0.2f, 0.2f )	// web 19 - dark grey
};

// forward reference
void ShutdownSound();

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common)
{
    // Exit back to PSP OS
	ShutdownSound();
	sceGuTerm();
	sceKernelExitGame();
	return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();

	return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}



// Start up the sound system
void StartupSound()
{
	sceAudioChReserve(0, 8192, PSP_AUDIO_FORMAT_MONO);				// for player laser (quick sample retrigger)
	sceAudioChReserve(1, 8192, PSP_AUDIO_FORMAT_MONO);				// for enemy fire
	sceAudioChReserve(2, 32768, PSP_AUDIO_FORMAT_MONO);				// for enemy actions
	sceAudioChReserve(3, 32768, PSP_AUDIO_FORMAT_MONO);				// for player actions
	sceAudioChReserve(4, 32768, PSP_AUDIO_FORMAT_MONO);				// for notifications

/* commented out JH 25/12/2007
	for (int i = 0; i < PSP_AUDIO_CHANNEL_MAX; i++)
		{
		sceAudioChReserve(i, 32768, PSP_AUDIO_FORMAT_MONO);
		//sceAudioChReserve(2, PSP_AUDIO_SAMPLE_MAX, PSP_AUDIO_FORMAT_MONO);
		}
*/
}

// Play a sample on indicated channel (0 to 3)
void WFPlaySound(int channel, int sampleIndex)
{
	void *sp = NULL;
	int samplecount = 8192;
	switch(sampleIndex)
		{
/*
#include "smp_laser2.h"
#include "smp_alienninja.h"
#include "smp_squarelead.h"
//#include "smp_explode4.h"
#include "smp_symsynth.h"
//#include "smp_enemyexplode.h"
//#include "smp_boef.h"
//#include "smp_wow2.h"
#include "smp_bass.h"
//#include "smp_onweb.h" 
//#include "smp_grs4.h"
#include "smp_pwawa.h"
#include "smp_quark2.h"
#include "smp_1up.h"
#include "smp_explode.h"
#include "smp_bleep.h"
#include "smp_bomb.h"
*/

		case SAMPLE_PLAYERFIRE :	sp = (void *)smp_laser2;		samplecount = size_smp_laser2; break;
		case SAMPLE_PLAYEREXPLODE : sp = (void *)smp_bomb;			samplecount = size_smp_bomb; break;
		case SAMPLE_PLAYERCAUGHT :	sp = (void *)smp_squarelead;	samplecount = size_smp_squarelead; break;
		//case SAMPLE_ENEMYEXPLODE :	sp = (void *)smp_quark2;		samplecount = size_smp_quark2; break;
		case SAMPLE_ENEMYEXPLODE :	sp = (void *)smp_bass;			samplecount = size_smp_bass; break;
		case SAMPLE_TANKERSPAWN :	sp = (void *)smp_alienninja;	samplecount = size_smp_alienninja; break;
		//case SAMPLE_SPIKEHIT :		sp = (void *)smp_bass;			samplecount = size_smp_bass; break;
		case SAMPLE_SPIKEHIT :		sp = (void *)smp_quark2;		samplecount = size_smp_quark2; break;
		case SAMPLE_FLIPPERFLIP :	sp = (void *)smp_laser2;		samplecount = size_smp_laser2; break;
		case SAMPLE_ZAPPERZAP :		sp = (void *)smp_laser2;		samplecount = size_smp_laser2; break;
		case SAMPLE_ENEMYFIRE :		sp = (void *)smp_choo;			samplecount = size_smp_choo; break;
		case SAMPLE_UFOFIRE :		sp = (void *)smp_laser2;		samplecount = size_smp_laser2; break;
		case SAMPLE_LEVELCOMPLETE : sp = (void *)smp_laser2;		samplecount = size_smp_laser2; break;
		case SAMPLE_LEVELSTART :	sp = (void *)smp_squarelead;	samplecount = size_smp_squarelead; break;
		case SAMPLE_ENEMYONWEB :	sp = (void *)smp_symsynth;		samplecount = size_smp_symsynth; break;
		case SAMPLE_ENEMYONRIM :	sp = (void *)smp_symsynth;		samplecount = size_smp_symsynth; break;
		case SAMPLE_CHARGEFULL :	sp = (void *)smp_pwawa;			samplecount = size_smp_pwawa; break;
		case SAMPLE_BARRIERHIT :	sp = (void *)smp_laser2;		samplecount = size_smp_laser2; break;
		case SAMPLE_1UP :			sp = (void *)smp_1up;			samplecount = size_smp_1up; break;
		case SAMPLE_BOMB :			sp = (void *)smp_explode;		samplecount = size_smp_explode; break;
		case SAMPLE_BLEEP :			sp = (void *)smp_bleep;			samplecount = size_smp_bleep; break;
		}

	if (sp)
		{
		//sceAudioSetChannelDataLen(channel, samplecount);
		sceAudioOutput(channel, PSP_AUDIO_VOLUME_MAX/2, sp);
		//sceAudioOutput(PSP_AUDIO_NEXT_CHANNEL, PSP_AUDIO_VOLUME_MAX/2, sp);
		}
}

// Tell sound driver to update audio stream
void UpdateSound()
{
	// Nothing to do
}

void ShutdownSound()
{
	// Release voices back to the OS
	sceAudioChRelease(0);
	sceAudioChRelease(1);
	sceAudioChRelease(2);
	sceAudioChRelease(3);
	sceAudioChRelease(4);
}


// prim buffer for GU
//static unsigned int __attribute__((aligned(16))) list[262144];
static unsigned int __attribute__((aligned(16))) displayList[131072];

// Setup PSP GU global parameters
int InitGU()										// All Setup For OpenGL Goes Here
{
	printf("InitGU()\n");

	// setup GU
	void* fbp0 = getStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888);
	void* fbp1 = getStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_8888);
	void* zbp = getStaticVramBuffer(BUF_WIDTH,SCR_HEIGHT,GU_PSM_4444);

	sceGuInit();

	sceGuStart(GU_DIRECT, displayList);
	sceGuDrawBuffer(GU_PSM_8888,fbp0,BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,fbp1,BUF_WIDTH);
	sceGuDepthBuffer(zbp,BUF_WIDTH);
	sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
	sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
	sceGuDepthRange(65535,0);
	sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	//sceGuEnable(GU_DEPTH_TEST);
	//sceGuFrontFace(GU_CCW);
	sceGuShadeModel(GU_FLAT);
	//sceGuEnable(GU_CULL_FACE);
	//sceGuEnable(GU_TEXTURE_2D);
	sceGuEnable(GU_CLIP_PLANES);
	//sceGuEnable(GU_LIGHTING);
	//sceGuEnable(GU_LIGHT0);
	sceGuFinish();
	sceGuSync(0,0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);

	//Scene.GetBackColour();
	sceGuClearColor(0xff008000);

	// make sure the VIEW matrix is initialised
	sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();

	return TRUE;										// Initialization Went OK
}

struct PSP_Vertex
{
	//float u, v;
	unsigned int color;
	ScePspFVector3 normal;
	ScePspFVector3 position;
};

static PSP_Vertex __attribute__((aligned(16))) vertex[1000];

// Swap video buffers
void flip(void)
{
	// PLATFORM-DEPENDANT!
	// Flush, vsync, Swap buffers
	sceGuFinish();
	sceGuSync(0,0);
	sceDisplayWaitVblankStart();
	sceGuSwapBuffers();
}

// Setup the display for drawing 2D (eg: for 2D vector text)
void Setup2DOverlay(void)
{
/* TODO: convert to GU
    glViewport( 0, 0, VIEW_WIDTH, VIEW_HEIGHT );
    glScissor( 0, 0, VIEW_WIDTH, VIEW_HEIGHT );
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	gluOrtho2D(0, VIEW_WIDTH, VIEW_HEIGHT, 0);			// left, right, bottom, top


	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
*/
	sceGuDisable(GU_DEPTH_TEST);
	sceGuDisable(GU_LIGHTING);
	
	// make sure we're in projection matrix mode 
	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();
	//void sceGumOrtho(float left, float right, float bottom, float top, float near, float far);
	sceGumOrtho(0, VIEW_WIDTH, VIEW_HEIGHT, 0, -10.0f, 10.0f);

	// make sure we're in model matrix mode
	sceGumMatrixMode(GU_MODEL);
	sceGumLoadIdentity();
}

// text colour
int g_textColour = 0xFFFFFFFF;

// Draw a scaled character to the current GU context
void DrawVectorGlyph(char c, int x, int y, float scale)
{
	int i;

	// convert ascii value to our glyph data index 
	c -= 32;

	if(c > 90)
		return;

	// Setup translation and scaling of the glyph
	sceGumMatrixMode(GU_MODEL);
	sceGumLoadIdentity();
	ScePspFVector3 v;
	v.x = (float)x; v.y = (float)y; v.z = 0.0f;
	ScePspFVector3 s;
	s.x = scale; s.y = scale; s.z = 0.0f;
	sceGumTranslate(&v);
	sceGumScale(&s);
	// output lines to GL
	VECTORGLYPH *glyph = &vecGlyph[(int)c];
	int ls, le;
	int m = 0;
	for(i=0; i < glyph->numlines; i++) {
		ls = glyph->ls[i];
		le = glyph->le[i];
		vertex[m].position.x = glyphPoints[ls].x;
		vertex[m].position.y = glyphPoints[ls].y;
		vertex[m].position.z = 0.0f;
		vertex[m].color = g_textColour;
		m++;
		vertex[m].position.x = glyphPoints[le].x;
		vertex[m].position.y = glyphPoints[le].y;
		vertex[m].position.z = 0.0f;
		vertex[m].color = g_textColour;
		m++;
	}

	// draw the lines, starting from vertex n
	sceKernelDcacheWritebackAll();
	sceGumDrawArray(GU_LINES, GU_COLOR_8888 | GU_NORMAL_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D, m, 0, &vertex[0]);
}

// Draw a string of scaled vector characters to the current GU context
void DrawVectorText(char *string, int x, int y, float scale, int spacing)
{
	while ((x < VIEW_WIDTH) && *string)
		{
		if(*string > 32)
			DrawVectorGlyph(*string, x, y, scale);
		string++;
		x += ((int)(scale * 1.2f) + spacing);
		}
}

// Draw a scaled character to the current GL context
void DrawChargeMeter()
{
	sceGumMatrixMode(GU_MODEL);
	sceGumLoadIdentity();

	int m = 0;

	// draw charge meter outline
	int color = MGREY;		// BGR
	vertex[m].position.x = 4.0f;
	vertex[m].position.y = VIEW_HEIGHT-4;
	vertex[m].position.z = 0.0f;
	vertex[m].color = color;
	m++;
	vertex[m].position.x = 4.0f;
	vertex[m].position.y = VIEW_HEIGHT-16;
	vertex[m].position.z = 0.0f;
	vertex[m].color = color;
	m++;
	vertex[m].position.x = 4.0f;
	vertex[m].position.y = VIEW_HEIGHT-4;
	vertex[m].position.z = 0.0f;
	vertex[m].color = color;
	m++;
	vertex[m].position.x = 104.0f;
	vertex[m].position.y = VIEW_HEIGHT-4;
	vertex[m].position.z = 0.0f;
	vertex[m].color = color;
	m++;
	vertex[m].position.x = 4.0f;
	vertex[m].position.y = VIEW_HEIGHT-16;
	vertex[m].position.z = 0.0f;
	vertex[m].color = color;
	m++;
	vertex[m].position.x = 104.0f;
	vertex[m].position.y = VIEW_HEIGHT-16;
	vertex[m].position.z = 0.0f;
	vertex[m].color = color;
	m++;
	vertex[m].position.x = 104.0f;
	vertex[m].position.y = VIEW_HEIGHT-4;
	vertex[m].position.z = 0.0f;
	vertex[m].color = color;
	m++;
	vertex[m].position.x = 104.0f;
	vertex[m].position.y = VIEW_HEIGHT-16;
	vertex[m].position.z = 0.0f;
	vertex[m].color = color;
	m++;

	// fill charge meter
	int k = 50 + g_charge*2;
	color = 0xFF600000 | (k << 8) | (k << 16);	// BGR
	for(unsigned int i=0; i<g_charge; i+=2)
		{
		vertex[m].position.x = (float)(4 + i);
		vertex[m].position.y = VIEW_HEIGHT-4;
		vertex[m].position.z = 0.0f;
		vertex[m].color = color;
		m++;
		vertex[m].position.x = (float)(4 + i);
		vertex[m].position.y = VIEW_HEIGHT-16;
		vertex[m].position.z = 0.0f;
		vertex[m].color = color;
		m++;
		}


	// draw the lines, starting from vertex 0
	sceKernelDcacheWritebackAll();
	sceGumDrawArray(GU_LINES, GU_COLOR_8888 | GU_NORMAL_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_3D, m ,0 ,&vertex[0]);
}

// populate the star list
void CreateStars(int numstars)
{
#ifdef DO_STARS
	E3D_Object *pObject;

	for(int i = 0; i<numstars; i++)
		{
		pObject = Scene.AddObject(STAR_LIST, "star");
		pObject->SetPosition((float)(rand()%STARFIELD_WIDTH - STARFIELD_WIDTH/2),
							(float)(rand()%STARFIELD_WIDTH - STARFIELD_WIDTH/2),
							(float)(-rand()%STARFIELD_WIDTH));
		pObject->behaviour = BEHAVE_NONE;
		}
#endif
}

// Update the stars for 1 tick
void MoveStars(bool turbo)
{
#ifdef DO_STARS
	float velocity = turbo ? 12.0f : 5.0f;
	float zscale = turbo ? 10.0f : 1.0f;
	OBJLIST* objList = Scene.GetObjectList(STAR_LIST);
	OBJLIST::iterator objIter;
	for(objIter = objList->begin(); objIter != objList->end(); objIter++)
		{
		objIter->position.z += velocity;
		if(objIter->position.z > 0.0f)
			objIter->position.z -= STARFIELD_WIDTH;
		objIter->scale.z = zscale;
		}
#endif
}


void CreateTitleScene(void)
{
	E3D_Object *pObject;
	Scene.RemoveAllObjects();

	// webfest title instance(s)
	for(int i = 0; i < 6; i++)
		{
		pObject = Scene.AddObject(PLAYER_LIST, "webfest");
		pObject->SetScale(2.0f, 2.0f, 2.0f);
		pObject->SetPosition(0.0f, 0.7f, -200.0f - 20.0f * i);
		}
}

void DrawTitleScreen(int frame)
{
	//float sf;

	// 3D stuff
	for(int i = 0; i < 6; i++)
		{
		E3D_Object *pObject = Scene.GetObject(PLAYER_LIST, i);
		pObject->position.z += 2.0f;
		if (i == 5 && pObject->position.z > 23.0f)
			pObject->position.z = 23.0f;
		}

	Scene.Render(VIEW_WIDTH, VIEW_HEIGHT, true);

	// Text overlay
	Setup2DOverlay();
	if (frame > 170)
		{
		g_textColour = BLUE;
		DrawVectorText("By Jum Hig 2007", VIEW_WIDTH/2 - GLYPH_WIDTH*8, 220, GLYPH_WIDTH, 0);
		}
	//glColor3f(1.0f,1.0f,0.3f);
	g_textColour = YELLOW;
	DrawVectorText("Press fire to start", VIEW_WIDTH/2 - GLYPH_WIDTH*6, 260, (float)(GLYPH_WIDTH * 0.6), 0);
}

// Draw the high score screen
void DrawHiScoreScreen(int frame)
{
	//float sf;
	//sf = 3.0f + cosf((float)frame / 10.0f) * 0.1f;

	// Do 3D stuff here
	Scene.Render(VIEW_WIDTH, VIEW_HEIGHT, true);

	// Text overlay
	Setup2DOverlay();

	//glColor3f(1.0f,1.0f,1.0f);
	g_textColour = WHITE;
	DrawVectorText("- HIGH SCORES -", VIEW_WIDTH/2 - 100, 20, GLYPH_WIDTH, 0);

	char stemp[100];
	for(int i=0; i<10; i++) {
		//glColor3ubv((GLubyte *)&colourtable[i+coffset]);
		sprintf(stemp, "%02d   %-10s   %06d  %d", i+1,
					HiScore_GetName(i),
					HiScore_GetScore(i),
					HiScore_GetLevel(i));
		DrawVectorText(stemp, 48, 60 + i*20, GLYPH_WIDTH, 0);
	}

	//glColor3f(1.0f,1.0f,0.3f);
	g_textColour = YELLOW;
	DrawVectorText("Press fire to start", VIEW_WIDTH/2 - GLYPH_WIDTH*6, 260, (float)(GLYPH_WIDTH * 0.6), 0);
}

// ******************** HISCORE ENTRY *********************
void DoHiscoreEntry(unsigned int score, unsigned int level)
{
	SceCtrlData pad;				// Joystick buffer
	//unsigned char r, g, b;
	//char s[256];
	char name[16] = "A";
	//float title_scale = -4.0;
	int namelen = 0;

	if(!HiScore_Check(score))			// not a high score
		return;
	
	// Start sound and MOD stuff if not already playing
	//WFPlaySound(SND_HISCORE);

	// clear out 3D scene
	Scene.RemoveAllObjects();
	CreateStars(60);

	// main loop
    bool quit = false;
    unsigned int flasher = 0;
	//unsigned int counter = 0;
	int debounce = 8;
#ifdef DEBUG
    printf("\nHiscore loop...");
#endif
	while (!quit)
		{
		// Get analog joystick position
		sceCtrlReadBufferPositive(&pad, 1); 

		sceGuStart(GU_DIRECT, displayList);

		// clear buffer
		Scene.Render(VIEW_WIDTH, VIEW_HEIGHT, true);

		// Text overlay
		Setup2DOverlay();

		// handle pad
		if(pad.Buttons & PSP_CTRL_CROSS)
			{
			if(0 == debounce)
				{
				namelen++;
				WFPlaySound(0, SAMPLE_BLEEP);
				name[namelen] = 'A';
				debounce = 20;
				}
			}

		if(pad.Buttons & PSP_CTRL_UP)
			{
			if(0 == debounce)
				{
				name[namelen]++;
				if(name[namelen] > 'Z') name[namelen] = 'A';
				//WFPlaySound(SND_LASER4);
				debounce = 20;
				}
			}

		if(pad.Buttons & PSP_CTRL_DOWN)
			{
			if(0 == debounce)
				{
				name[namelen]--;
				if(name[namelen] < 'A') name[namelen] = 'Z';
				//WFPlaySound(SND_LASER2);
				debounce = 20;
				}
			}

		if(namelen > 2)
			{
			name[3] = 0;
			WFPlaySound(4, SAMPLE_1UP);
			// add this score[0] and name to the high-score[0] table
			HiScore_Add(score, name, level, 0);
			//SPU_NoteOff(0, SND_HISCORE);
			HiScore_Save();
			quit = 1;
			}
		
		//r = (colourtable[flasher & 255] >> 17) & 0x7F;
		//g = (colourtable[flasher & 255] >> 9) & 0x7F;
		//b = (colourtable[flasher & 255] >> 1) & 0x7F;

		// Instructions
		//glColor3f(1.0f,0.5f,1.0f);
		g_textColour = PURPLE;
		DrawVectorText("CONGRATULATIONS!", VIEW_WIDTH/2 - GLYPH_WIDTH*8, 36, GLYPH_WIDTH, 0);
		//glColor3f(0.5f,0.5f,1.0f);
		g_textColour = BLUE;
		DrawVectorText(" YOU HAVE A HIGH SCORE!", VIEW_WIDTH/2 - GLYPH_WIDTH*12, 60, GLYPH_WIDTH, 0);
		DrawVectorText("PLEASE ENTER YOUR INITIALS:", VIEW_WIDTH/2 - GLYPH_WIDTH*14, 100, GLYPH_WIDTH, 0);
		DrawVectorText("---", VIEW_WIDTH/2 - GLYPH_WIDTH, 160, GLYPH_WIDTH, 0);
		DrawVectorText("PRESS UP/DOWN TO SELECT LETTER.", VIEW_WIDTH/2 - GLYPH_WIDTH*17, 240, GLYPH_WIDTH, 0);
		DrawVectorText("PRESS X TO ENTER LETTER.", VIEW_WIDTH/2 - GLYPH_WIDTH*12, 260, GLYPH_WIDTH, 0);
		DrawVectorText(name, VIEW_WIDTH/2 - GLYPH_WIDTH, 150, GLYPH_WIDTH, 0);

		//// draw frame
		////		DrawEmptyBox(3, 64, 64, SCREEN_WIDTH-64, SCREEN_HEIGHT-64, 200, 200, 200);
		//glColor3f(1.0f,1.0f,1.0f);
		//glBegin(GL_LINES);
		//	glVertex3f(64.0f, 64.0f, 0.0f);
		//	glVertex3f(VIEW_WIDTH-64, 64.0f, 0.0f);
		//	glVertex3f(64.0f, VIEW_HEIGHT-64, 0.0f);
		//	glVertex3f(VIEW_WIDTH-64, VIEW_HEIGHT-64, 0.0f);
		//	glVertex3f(64.0f, 64.0f, 0.0f);
		//	glVertex3f(64.0f,VIEW_HEIGHT-64, 0.0f);
		//	glVertex3f(VIEW_WIDTH-64, 64.0f, 0.0f);
		//	glVertex3f(VIEW_WIDTH-64,VIEW_HEIGHT-64, 0.0f);
		//glEnd();

		// Flush, vsync, Swap buffers
		flip();

		MoveStars(false);

		if(debounce)
			debounce--;
		
		// update counter used for flasher and timeout
		flasher++;
		//if(flasher == 600) quit = 1;		// timeout title
		// update MOD
		//PollMOD(&themod);
		}	// wend !quit

}

// Draw the Pause text and wait for unpause
void DoPause(void)
{
	char msg[256];
	SceCtrlData pad;				// Joystick buffer

	scePowerSetCpuClockFrequency(33);
	scePowerSetBusClockFrequency(33);

	bool paused = true;
	while(paused)
		{
		// Read controller
		sceCtrlReadBufferPositive(&pad, 1); 

		sceGuStart(GU_DIRECT, displayList);

		// clear buffer
		Scene.Render(VIEW_WIDTH, VIEW_HEIGHT, true);

		// Text overlay
		Setup2DOverlay();

		//glColor3f(1.0f,0.2f,0.0f);
		g_textColour = RED;
		DrawVectorText("--- PAUSED ---", VIEW_WIDTH/2 - 100, 120, GLYPH_WIDTH, 0);

		if (scePowerIsBatteryExist())
			{
			if (scePowerIsLowBattery())
				DrawVectorText("BATTERY LOW!", GLYPH_WIDTH, GLYPH_WIDTH, GLYPH_WIDTH, 0);
			}

		g_textColour = WHITE;
		sprintf(msg, "CPU %d MHz", scePowerGetCpuClockFrequency());
		DrawVectorText(msg, GLYPH_WIDTH, GLYPH_WIDTH*2, GLYPH_WIDTH, 0);
		sprintf(msg, "BUS %d MHz", scePowerGetBusClockFrequency());
		DrawVectorText(msg, GLYPH_WIDTH, GLYPH_WIDTH*3, GLYPH_WIDTH, 0);

		// Swap GL buffers
		flip();

		// Check if fire pressed
		if(pad.Buttons & PSP_CTRL_CROSS)
			paused = false;
		}

	// restore clock speed
	scePowerSetClockFrequency(222, 222, 111);
}

// Update the players score
void AddScore(unsigned int value)
{
	g_score += value;

	// then award bonus / extra life etc if neccessary
	if(g_score > g_nextBonusScore)
		{
		// Play "1UP" sound
		WFPlaySound(4, SAMPLE_1UP);
		g_lives++;
		if (20000 == g_nextBonusScore)
			g_nextBonusScore = 50000;
		else
			g_nextBonusScore += 50000;
		}
}

void AddCharge(int value)
{
	// Don't do anything if charge full and we are adding to it
	if (CHARGE_FULL == g_charge && value > 0)
		return;

	// Pump up charge meter (or take away from it)
	g_charge += value;
	g_charge = max(g_charge, 0);

	if (g_charge > CHARGE_FULL)
		{
		WFPlaySound(4, SAMPLE_CHARGEFULL);
		g_charge = CHARGE_FULL;
		}
}

#define TUBE_ZSTEPS 4

/// Make a 3D model of the specified tube
void makeTubeModel2(E3D_Model *model, TUBEDATA *tube, Colour colour)
{
	int i, j;
	model->Reset();
 
	// we could just create "near" and "far" vertices, but for now 
	// we have to create a "mesh" of vertices between "near" and "far",
	// so that PSP does not have a clipping problem when player zooms
	// down the tube
	float dz = TUBE_LENGTH / TUBE_ZSTEPS;		// mesh step size in z direction
	for(j = 0; j < (TUBE_ZSTEPS+1); j++) {
		for(i=0; i<tube->numVertices; i++) {
			Vertex v( tube->vertex[i].x * STEP_SIZE, tube->vertex[i].y * STEP_SIZE, -(dz * j));
			v.SetColour(colour.r, colour.g, colour.b);
			model->AddVertex(v);
		}
	}

	// create "near" edge lines and "far" edge lines for the tube mesh
	for(i=0; i<tube->numVertices-1; i++) {
		int startVert = i;
		int endVert = i+1;
		Line *line = model->AddLine(startVert, endVert);
		line->colour = colour;
		startVert = i + (tube->numVertices * TUBE_ZSTEPS);
		endVert = i + (tube->numVertices * TUBE_ZSTEPS) + 1;
		line = model->AddLine(startVert, endVert);
		line->colour = colour;
	}
	// create line sgements from near edge to far edge
	for(i=0; i<tube->numVertices; i++) {
		for(j = 0; j < TUBE_ZSTEPS; j++) {
			int startVert = i + (j * tube->numVertices);
			int endVert = i + ((j+1) * tube->numVertices);
			Line *line = model->AddLine(startVert, endVert);
			line->colour = colour;
		}
	}

/* old
	for(i=0; i<tube->numVertices; i++) {
		int startVert = tube->numVertices * 3;
		int endVert = i + tube->numVertices;
		Line *line = model->AddLine(startVert, endVert);
		line->colour = colour;
	}

	for(i=0; i<tube->numVertices-1; i++) {
		int startVert = i;
		int endVert = i+1;
		Line *line = model->AddLine(startVert, endVert);
		line->colour = colour;
		startVert = i + tube->numVertices;
		endVert = i + tube->numVertices + 1;
		line = model->AddLine(startVert, endVert);
		line->colour = colour;
	}
*/
}

/// Get the centre and angle of a tube segment
/// @param[out] centre			Calculated centre point of the segment
/// @return						Angle of the segment
float GetTubeSegmentCentreAngle(TUBEDATA *tube, int segment, Vector &centre) {
	if(segment >= tube->numVertices - 1)
		printf("Error: segment out of range!\n");
	Vector v1 = tube->vertex[segment];
	Vector v2 = tube->vertex[(segment+1) % tube->numVertices];
	centre.x = (v1.x+v2.x)*STEP_SIZE*0.5f;
	centre.y = (v1.y+v2.y)*STEP_SIZE*0.5f;
	centre.z = 0.0f;
	Vector baseLine(v2.x - v1.x, v2.y - v1.y, 0.0);
	float angle = (float)atan2(baseLine.y, baseLine.x);
	return angle;
}

// Find first free player bullet and launch it
void FireBullet(Vector &pos, Vector &velocity) {
	OBJLIST* objList = Scene.GetObjectList(BULLET_LIST);
	OBJLIST::iterator iter;
	for(iter = objList->begin(); iter != objList->end(); iter++)
		{
		if(0 == iter->behaviour)
			{
			iter->behaviour = BEHAVE_NORMAL;
			iter->counter = 80;
			iter->position = pos;
			iter->velocity = velocity;
			break; //i = BULLET_ENDID + 1;
			}
		}
}

// Activate smart bomb
void FireSmartBomb() {
	OBJLIST* objList = Scene.GetObjectList(ENEMY_LIST);
	OBJLIST::iterator enemyIter;
	for(enemyIter = objList->begin(); enemyIter != objList->end(); enemyIter++)
		{
		if(enemyIter->behaviour > BEHAVE_NONE)
			{
			if(enemyIter->position.z > -TUBE_LENGTH)
				{
				Vector v(0.0f, 0.0f,0.0f);
				AddFrags(enemyIter->position, v, 8);
				enemyIter->SetPosition(OFFSCREEN);
				enemyIter->behaviour = BEHAVE_NONE;		// switch off enemy
				}
			}
		}
}

// Get web colour for a specific level
Colour GetWebColour(int level)
{
	Colour col;
	// tube colour per level, or random
	if(level < NUM_LEVELS)
		col = webColour[level];
	else
		{
        col.r = 0.0;	//
		col.g = 0.0;
		col.b = 1.0;
		}

	return col;
}

/// Populate the 3D scene, given the level (stage)
int CreateScene(unsigned int level, TUBEDATA *tube) {
	// NB: we pre-fill all the "object slots" we are
	//     going to use so that we don't have to generate
	//     objects on the fly
	int i;
	char webFilename[2000];
	E3D_Object *pObject = NULL;

	Scene.RemoveAllObjects();

	// load the web shape
	GetWebFilename(level % NUM_LEVELS, webFilename);
	LoadWeb(webFilename, tube);

	// create the tube model
	E3D_Model *tubeModel = Scene.GetModel(TUBE_MODEL);
	// Get tube colour for this level
	Colour colour = GetWebColour(level);
	// Make the tube
	makeTubeModel2(tubeModel, tube, colour);

	// Calculate enemy speed scale factor so that enemy speed is doubles
	// by level 10 and tripled by level 20 (limit to 3x)
	float speedScale = 1.0f + ((float)level * 0.1f);
	if (speedScale > 3.0f)
		speedScale = 3.0f;

	// The tube/web
	pObject = Scene.AddObject(PLAYER_LIST, "tube");
	pObject->SetPosition(0.0f, 0.0f, 0.0f);
	pObject->behaviour = BEHAVE_NONE;

	// player 1 and 2
	pObject = Scene.AddObject(PLAYER_LIST, "player1");				// player 1
	pObject->SetPosition(0.0f, 0.0f, 0.0f);
	pObject->behaviour = BEHAVE_NONE;
	//pObject = Scene.AddObject(PLAYER_LIST, "player2");				// player 2
	//pObject->SetPosition(0.0f, 1000.0f, 0.0f);			// offscreen
	//pObject->behaviour = BEHAVE_NONE;

	// create the bullet objects
	for(i = 0; i < 20; i++) {
		pObject = Scene.AddObject(BULLET_LIST, "bullet1");
		pObject->SetPosition(0.0f, 1000.0f, 0.0f);				// offscreen
	}

	// create the frag objects
	for(i = 0; i < 60; i++) {
		pObject = Scene.AddObject(FRAG_LIST, "frag");
		pObject->SetPosition(0.0f, 1000.0f, 0.0f);				// offscreen
	}


	// manufacture the enemies for this level
	ENEMYSPAWNDATA *spawnData = &enemySpawnData[level % NUM_LEVELS];		// we only have data for 20 levels!!!
	// Flippers
	for(i = 0; i < spawnData->numFlippers; i++) {
		// calculate enemy position and rotation
		int seg = rand() % (tube->numVertices - 1);
		Vector p;
		float angle = GetTubeSegmentCentreAngle(tube, seg, p);
		p.z = -(TUBE_LENGTH*4) - (rand() % SPAWNPERIOD);

		pObject = Scene.AddObject(ENEMY_LIST, "flipper");
		pObject->position = p; 
		pObject->SetRotation(0.0, 0.0, angle);
		pObject->SetVelocity(0.0, 0.0, 0.3f * speedScale); 
		pObject->var1 = (short)seg;		// current segment
		pObject->energy = 0;
		pObject->energymax = 50;		// time till flip
		pObject->counter = 200 + (rand() % 200) - min(level * 10, 150);
		pObject->behaviour = BEHAVE_NORMAL;
	}

	// Tankers
	for(i = 0; i < spawnData->numTankers; i++) {
		// calculate enemy position and rotation
		int seg = rand() % (tube->numVertices - 1);
		Vector p;
		float angle = GetTubeSegmentCentreAngle(tube, seg, p);
		p.z = -(TUBE_LENGTH*4) - (rand() % SPAWNPERIOD);

		pObject = Scene.AddObject(ENEMY_LIST, "tanker");
		pObject->position = p; 
		pObject->SetRotation(0.0, 0.0, angle);
		pObject->SetVelocity(0.0, 0.0, 0.2f * speedScale); 
		pObject->var1 = (short)seg;
		pObject->counter = 0;
		pObject->behaviour = BEHAVE_NORMAL;
	}
	
	// Spikers
	for(i = 0; i < spawnData->numSpikers; i++) {
		// calculate enemy position and rotation
		int seg = rand() % (tube->numVertices - 1);
		Vector p;
		float angle = GetTubeSegmentCentreAngle(tube, seg, p);
		p.z = -(TUBE_LENGTH * 1.2f) - (rand() % (int)TUBE_LENGTH * speedScale);

		pObject = Scene.AddObject(ENEMY_LIST, "spike1");
		pObject->position = p;
		pObject->velocity = Vector(0.0f, 0.0f, 0.2f * speedScale);		// initial spike velocity
		//pObject->SetRotation(0.0, 0.0, angle);
		pObject->counter = 0;
		pObject->behaviour = BEHAVE_NORMAL;
	}	
/*
	// Fuseballs
	for(i = 0; i < spawnData->numFuseballs; i++) {
		// calculate enemy position and rotation
		int seg = rand() % (tube->numVertices - 1);
		Vector p;
		float angle = GetTubeSegmentCentreAngle(tube, seg, p);
		p.z = -TUBE_LENGTH - (rand() % 400);

		pObject = Scene.AddObject(ENEMY_LIST, "fuseball");
		p.z = -TUBE_LENGTH;
		pObject->position = p; 
		//pObject->SetRotation(0.0, 0.0, angle);
		pObject->counter = 0;
		pObject->behaviour = BEHAVE_NORMAL;
	}	
*/
/*
	// Zappers
	for(i = 0; i < spawnData->numZappers; i++) {
		// calculate enemy position and rotation
		int seg = rand() % (tube->numVertices - 1);
		Vector p;
		float angle = GetTubeSegmentCentreAngle(tube, seg, p);
		p.z = -TUBE_LENGTH - (rand() % 400);

		pObject = Scene.AddObject(ENEMY_LIST, "fuseball");
		p.z = -TUBE_LENGTH;
		pObject->position = p; 
		//pObject->SetRotation(0.0, 0.0, angle);
		pObject->counter = 0;
		pObject->behaviour = BEHAVE_NORMAL;
	}	
*/
	// Barriers
	for(i = 0; i < spawnData->numBarriers; i++) {
		// calculate enemy position and rotation
		int seg = rand() % (tube->numVertices - 1);
		Vector p;
		float angle = GetTubeSegmentCentreAngle(tube, seg, p);
		p.z = -TUBE_LENGTH/3 - (rand() % (int)(TUBE_LENGTH * 0.666));

		pObject = Scene.AddObject(ENEMY_LIST, "barrier");
		pObject->position = p; 
		pObject->SetRotation(0.0, 0.0, angle);
		pObject->SetVelocity(0.0, 0.0, 0.1f); 
		pObject->counter = 0;
		pObject->behaviour = BEHAVE_NORMAL;
	}	

	// UFO's
	for(i = 0; i < spawnData->numUfos; i++) {
		// calculate enemy position and rotation
		int seg = rand() % (tube->numVertices - 1);
		Vector p;
		float angle = GetTubeSegmentCentreAngle(tube, seg, p);
		p.z = -(TUBE_LENGTH*8) - (rand() % SPAWNPERIOD * 2);

		pObject = Scene.AddObject(ENEMY_LIST, "ufo");
		pObject->position = p; 
		pObject->SetRotation(0.0, 0.0, angle);
		pObject->speed = 0.1f * speedScale; 
		pObject->var1 = (short)seg;		// current segment
		pObject->counter = 0;
		pObject->behaviour = BEHAVE_NORMAL;
	}	


	return i;
}

// Check if player has been hit by an enemy or bullet
bool IsPlayerHit()
{
	bool hit = false;

	E3D_Object *player = Scene.GetObject(PLAYER_LIST, PLAYER1_ID);

	OBJLIST* objList = Scene.GetObjectList(ENEMY_LIST);
	OBJLIST::iterator objIter;
	for(objIter = objList->begin(); objIter != objList->end(); objIter++)
		{
		if(objIter->behaviour != BEHAVE_NONE)
			{
			switch(objIter->modelNumber)
				{
				case EBULLET_MODEL :
				case FLIPPER_MODEL :
				case SPIKE1_MODEL :
				case SPIKE2_MODEL :
				case BARRIER_MODEL :
					{
					if(fabs(objIter->position.z - player->position.z) < HIT_RADIUS)
						{
						if(fabs(objIter->position.x - player->position.x) < HIT_RADIUS)
							{
							if(fabs(objIter->position.y - player->position.y) < HIT_RADIUS)
								{
								// player hit!
								// if we have full charge, then treat it as a shield
								if (g_charge == CHARGE_FULL)
									{
									WFPlaySound(3, SAMPLE_PLAYEREXPLODE);
									// 1. create frags
									Vector v(0.0f, 0.0f, 0.0f);
									AddFrags(objIter->position, v, 25);
									// 2. kill enemy
									objIter->SetPosition(0.0f, 1000.0f, 0.0f);
									objIter->behaviour = BEHAVE_NONE;		// switch off enemy
									// 3. reset charge meter
									g_charge = 0;
									// 4. shake screen
									g_shakeCount = 20;
									}
								else
									{
									hit = true;
									}
								}
							}
						}
					}
				}
			}
		}

	return hit;

}

// See if all relevant enemies have been wasted
bool IsLevelComplete()
{
	bool finished = true;
	OBJLIST* objList = Scene.GetObjectList(ENEMY_LIST);
	OBJLIST::iterator objIter;
	for(objIter = objList->begin(); objIter != objList->end(); objIter++)
		{
		if(objIter->behaviour != BEHAVE_NONE)
			{
			if(objIter->modelNumber != SPIKE2_MODEL && objIter->modelNumber != BARRIER_MODEL)
				{
				finished = false;
				}
			}
		}

	return finished;
}

// Level run state
enum RUNSTATE {
	RUNSTATE_RUNNING		= 0,
	RUNSTATE_QUIT			= 1,	// player quit level
	RUNSTATE_LEVELCOMPLETE	= 2,	// player has completed level
	RUNSTATE_PLAYERKILLED	= 3		// played was killed
};

/////////////////////////////////////////////////////////////////////////////////
// Main loop
/////////////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[]) {
	unsigned int reloadCount = 0;
	unsigned int debounce = 0;
	bool gameOver = true;
	unsigned int level = 0;

	int	frames;
	//int x, y;
    //double  t, t0, fps;
    //char titlestr[500];
	E3D_Object *pObject;
	SceCtrlData pad;				// Joystick buffer
	TUBEDATA tube;					// Web data

	// Set up PSP 
	pspDebugScreenInit();
	//setupCallbacks();
	SetupCallbacks();

	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_DIGITAL); //PSP_CTRL_MODE_ANALOG);

	pspDebugScreenSetXY(0, 2);

	// initialise high-scores
	printf("Loading high scores...\n");
	HiScore_Init();
	HiScore_Load();

	printf("Setting up scene...\n");

	// Set up the E3D scene
	Scene.Start();
	Scene.SetRenderDistance(1000.0f);

	// Set up the scene's default light (light0)
	E3D_Light *light0 = Scene.GetLight(0);
	light0->SetColour( 0.8f, 0.8f, 0.6f );
	light0->SetAmbient( 0.4f, 0.4f, 0.4f );
	light0->SetDirection( 0.707f, 0.707f, 0.0f ); //0.0, 0.0, -1.0); 

	// Set up the scene's default camera
	E3D_Camera *camera = Scene.GetCamera(0);
	camera->SetPosition(-10.0f, 1.5f, 0.0f);
	camera->SetDirection(0.0f, 0.0f, 1.0f);
	camera->SetUpVector(0.0f, 1.0f, 0.0f);
	camera->SetFOV(60.0f);
	Vector cameraDest;				// running camera destination

	printf("Loading models... \n");

	// NB: Models must be added to the scene in the same order as the model map
	//     (See webfest.h)
	Scene.AddModel("tube");
	Scene.AddModel("player1", "models/player.obj");
	printf("player1 loaded. \n");
	Scene.AddModel("player2", "models/redblock.obj");
	printf("player2 loaded. \n");
	Scene.AddModel("bullet1", "models/bullet1.obj");
	printf("bullet1 loaded. \n");
	Scene.AddModel("frag", "models/frag3.obj");
	printf("frag loaded. \n");
	Scene.AddModel("flipper", "models/flipper.obj");
	printf("flipper loaded. \n");
	Scene.AddModel("tanker", "models/tanker.obj");
	printf("tanker loaded. \n");
	Scene.AddModel("ebullet", "models/enemy_bullet.obj");
	printf("ebullet loaded. \n");
	Scene.AddModel("spike1", "models/spike1.obj");
	printf("spike1 loaded. \n");
	Scene.AddModel("spike2", "models/spike2.obj");
	printf("spike2 loaded. \n");
	Scene.AddModel("barrier", "models/barrier.obj");
	printf("barrier loaded. \n");
	Scene.AddModel("ufo", "models/ufo.obj");
	printf("ufo loaded. \n");
	//Scene.AddModel("spiker", "models/spider.obj");
	Scene.AddModel("star", "models/star.obj");
	printf("star loaded. \n");
	Scene.AddModel("webfest", "models/webfest.obj");
	printf("title loaded. \n");

	printf("Models loaded. \n");

	// Set up our PSP GU environment
	printf( "InitGU() returned %d\n", InitGU() );

	// Setup sound
	StartupSound();

	// ********************* OUTER CONTROL LOOP ************************
	while(true)
		{
		// 1. Show title screen / hiscore until player start
		camera->SetPosition(0.0f, 0.5f, 30.0f);
		camera->SetDirection(0.0f, 0.0f, -1.0f);
		camera->SetUpVector(0.0f, 1.0f, 0.0f);
		frames = 0;
		level = DEBUG_START_LEVEL; 
		gameOver = true;
		while(gameOver)
			{
			// set up title screen
			CreateTitleScene();
			CreateStars(60);
			frames = 0;
			int testSampleIndex = 0;
			// Title screen loop
			while(frames < 400)
				{
				//sceCtrlReadBufferPositive(&pad, 1);
				sceCtrlPeekBufferPositive(&pad, 1);

				sceGuStart(GU_DIRECT, displayList);

				MoveStars(false);
				DrawTitleScreen(frames++);

				// Flush, vsync, Swap buffers
				flip();

				// check for start (but avoid start key immediately after hi-score enter)
				if((pad.Buttons & PSP_CTRL_CROSS) && frames > 100)
					{
					frames = 100000;
					gameOver = false;
					}

				// Check if user wants to exit game
				if(pad.Buttons & PSP_CTRL_START)
					{
					// Exit back to PSP OS
					ShutdownSound();
					sceGuTerm();
					sceKernelExitGame();
					return 0;					// Exit The Program
					}

				// Sample test
				if((pad.Buttons & PSP_CTRL_RTRIGGER) && !debounce)
					{
					WFPlaySound(0, testSampleIndex++);
					testSampleIndex = testSampleIndex % SAMPLE_MAX;
					debounce = 20;
					}

				if(debounce)
					debounce--;

				UpdateSound();
				}

			// set up hiscore screen
			Scene.RemoveAllObjects();
			CreateStars(60);
			while(frames < 800)
				{
				//sceCtrlReadBufferPositive(&pad, 1); 
				sceCtrlPeekBufferPositive(&pad, 1);

				sceGuStart(GU_DIRECT, displayList);

				DrawHiScoreScreen(frames++);

				// Flush, vsync, Swap buffers
				flip();

				MoveStars(false);

				// check for start
				if(pad.Buttons & PSP_CTRL_CROSS)
					{
					frames = 10000;
					gameOver = false;
					}

				// Check if user wants to exit game
				if(pad.Buttons & PSP_CTRL_START)
					{
					// Exit back to PSP OS
					ShutdownSound();
					sceGuTerm();
					sceKernelExitGame();
					return 0;					// Exit The Program
					}
				}
			} // wend title / hiscore loop

		// 2. Game Play Loop
		g_lives = 3;
		g_score = 0;
		g_nextBonusScore = 20000;
		int playerDying = 0;
		while(!gameOver)
			{
			// Create the relevant level in E3D
			// NB: we pre-fill all the "object slots" we are
			//     going to use so that we don't have to generate
			//     objects on the fly
			printf("CreateScene() start\n");
			CreateScene(level, &tube);
			printf("CreateScene() done\n");
			//CreateStars(100);
			camera->SetPosition((float)(rand()%600 - 300), (float)(rand()%600 - 300), 1600.0f);

			// Main loop
			int levelStart = 180;
			int currentSegment = tube.numVertices / 2; //tube.startingSegment;
			cameraDest.Set(0.0, 0.0, 20.0);
			g_shotsFired = 0;
			g_shotsHit = 0;
			WFPlaySound(4, SAMPLE_LEVELSTART);

			RUNSTATE runState = RUNSTATE_RUNNING;
			frames = 0;
			while( RUNSTATE_RUNNING == runState )
				{
				// get controller state
				//sceCtrlReadBufferPositive(&pad, 1); 
				sceCtrlPeekBufferPositive(&pad, 1);
				frames++;
		
				sceGuStart(GU_DIRECT, displayList);
		
				// Set player position and rotation
				float playerAngle;
				if(0 == playerDying)
					{
					pObject = Scene.GetObject(PLAYER_LIST, PLAYER1_ID);
					// interpolate player position towards "current" (target) segment
					Vector p;
					float segmentAngle = GetTubeSegmentCentreAngle(&tube, currentSegment, p);
					E3D::Vector v(p.x - pObject->position.x, p.y - pObject->position.y, 0.0f);
					if (v.GetLength() > 0.75f)
						v.SetLength(0.75f);
					pObject->position += v;
					// Interpolate player rotation towards "current" (target) segment
					playerAngle = pObject->rotation.z + ((segmentAngle - pObject->rotation.z) * 0.2f);
					pObject->SetRotation(0.0f, 0.0f, playerAngle);
					}

				// shake camera if neccessary
				if (g_shakeCount > 0)
					{
					g_shakeCount--;
					Vector p = camera->GetPosition();
					p += Vector(rand()%3-1, rand()%3-1, rand()%3-1) * 0.5f;
					camera->SetPosition(p);
					}

				// Behaviour engine - updates all our game objects for 1 frame
				DoBehaviours(&tube, level);
				//MoveStars(g_charge == CHARGE_FULL);

				// Tell E3D to render it's scene to the current GU context
				// TODO: blur effects, camera jiggle effects, strobe effects, etc
				Scene.Render((float)SCR_WIDTH, (float)SCR_HEIGHT, true);
				Scene.SetBackColour(0.0f, 0.0f, 0.0f);

				// Overlay 2D text on our display
				Setup2DOverlay();
				char s[20];
				if(levelStart > 0)
					{
					g_textColour = YELLOW;
					sprintf(s, "LEVEL %02d", level);
					DrawVectorText(s, VIEW_WIDTH/2 - 55, VIEW_HEIGHT/2 - 20, GLYPH_WIDTH, 0);
					// Get tube colour for this level
					Colour webcol = GetWebColour(level);
					g_textColour = ((int)(webcol.r*255) | (int)(webcol.g*255) << 8 | (int)(webcol.b*255) << 16);
					DrawVectorText(tube.name, VIEW_WIDTH/2 - strlen(tube.name)*GLYPH_WIDTH, VIEW_HEIGHT/2 + 40, GLYPH_WIDTH*2, 0);
					levelStart--;
					}
				else
					{
					g_textColour = WHITE;
					sprintf(s, "HISCORE %06d", HiScore_GetScore(0));
					DrawVectorText(s, VIEW_WIDTH - GLYPH_WIDTH*14, 8, GLYPH_WIDTH, 0);
					sprintf(s, "SCORE %06d", g_score);
					DrawVectorText(s, 16, 8, GLYPH_WIDTH, 0);
					// draw lives left
					memset(s, 0, 20);
					for(unsigned int i = 0; i< g_lives; i++)
						{
						s[i] = ';';			// ship glyph
						DrawVectorText(s, 16, 24, GLYPH_WIDTH, 0);
						}
					// draw game over message if neccessary
					if(0 == g_lives)
						{
						g_textColour = YELLOW;
						DrawVectorText("G A M E   O V E R", VIEW_WIDTH/2 - GLYPH_WIDTH * 9, VIEW_HEIGHT/2, GLYPH_WIDTH, 0);
						}

					// draw charge meter (flash if charged up)
					g_textColour = MGREY;
					DrawVectorText("CHARGE", 36, VIEW_HEIGHT - 10, GLYPH_WIDTH/2, 0);
					if(g_charge < CHARGE_FULL || 0 == ((frames/10)%2))
						DrawChargeMeter();
					}
				// 2nd part of end-of-level zoom - show shooting stats
				pObject = Scene.GetObject(PLAYER_LIST, PLAYER1_ID);
				if(pObject->position.z < -TUBE_LENGTH )
					{
					unsigned int hitPerc = ((g_shotsHit+1)*100)/(g_shotsFired+1);		// avoid DBZ exception
					g_textColour = GREEN;
					sprintf(s, "SHOOTING %d/%d [%d%%]", g_shotsHit, g_shotsFired, hitPerc);
					DrawVectorText(s, VIEW_WIDTH/2 - strlen(s)*GLYPH_WIDTH/2, VIEW_HEIGHT/4, GLYPH_WIDTH, 0);
					if (hitPerc == 100)
						{	
						AddScore(20000 / (int)(TUBE_LENGTH*2));
						g_textColour = BLUE;
						DrawVectorText("PERFECT!", VIEW_WIDTH/2 - 5*GLYPH_WIDTH/2, VIEW_HEIGHT/4 + 20, GLYPH_WIDTH, 0);
						DrawVectorText("BONUS 20000", VIEW_WIDTH/2 - 6*GLYPH_WIDTH/2, VIEW_HEIGHT/4 + 40, GLYPH_WIDTH, 0);
						}
					else if (hitPerc > 75)
						{
						AddScore(5000 / (int)(TUBE_LENGTH*2));
						g_textColour = BLUE;
						DrawVectorText("GOOD SHOOTING!", VIEW_WIDTH/2 - 10*GLYPH_WIDTH/2, VIEW_HEIGHT/4 + 20, GLYPH_WIDTH, 0);
						DrawVectorText("BONUS 5000", VIEW_WIDTH/2 - 6*GLYPH_WIDTH/2, VIEW_HEIGHT/4 + 40, GLYPH_WIDTH, 0);
						}
					}

				// Flush, vsync, Swap buffers
				flip();
				UpdateSound();

				if(debounce)
					debounce--;

				// update player
				if(0 == playerDying)
					{
					if(pad.Buttons & PSP_CTRL_LEFT)
						{
						//heading += 0.03;
						if(0 == debounce)
							{
							if(0 == currentSegment)
								{
								if(tube.closed)
									currentSegment = tube.numVertices-2;
								}
							else
								{
								currentSegment--;
								}
							debounce = 8;
							}
						}
					else if(pad.Buttons & PSP_CTRL_RIGHT)
						{
						//heading -= 0.03;
						if(0 == debounce)
							{
							if(currentSegment == tube.numVertices-2)
								{
								if(tube.closed)
									currentSegment = 0;
								}
							else
								{
								currentSegment++;
								}
							debounce = 8;
							}
						}

					// camera "up" is perpendicular to current segment base line
					//Vector baseLine(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
					Vector depthLine(0.0, 0.0, 1.0);
					Vector normal((float)-sin(playerAngle), (float)cos(playerAngle), 0.0f);
					//E3D_Utility::cross_product_f(depthLine, baseLine, &normal);
					//normal.Normalise();
					// rotate camera up vector towards normal (or slerp it)
					Vector camUpVec = camera->GetUpVector();
					Vector upDiff = normal - camUpVec;
					camUpVec += (upDiff * 0.1f);
					camUpVec.Normalise();
					camera->SetUpVector(camUpVec);

					Vector dirn;
					dirn.x = 0.0; //sin(heading);
					dirn.z = -1.0; //cos(heading);
					dirn.y = 0.0;
					camera->SetDirection(dirn);

					// set camera final destination point
					pObject = Scene.GetObject(PLAYER_LIST, PLAYER1_ID);
					Vector playerPos = pObject->position;
					//float vecLen = playerPos.GetLength();
					//cameraDest.Set(playerPos.x*0.1f, playerPos.y*0.1f, playerPos.z + 20.0f + (vecLen * 0.1f));
					cameraDest = playerPos + camUpVec * 10;
					cameraDest.z += 20.0f; // + (vecLen * 0.1f)
					// float camera towards final destination point
					Vector camPos = camera->GetPosition();
					Vector diff = cameraDest - camPos;
					//fprintf(stderr, "diff: %.3f\n", diff.GetLength());
					if(diff.GetLength() > 120.0f)		
						diff.SetLength(120.0f);				// make sure we don't move too fast
					camPos += (diff * 0.1f);
					camera->SetPosition(camPos);

					// check fire button
					if(pad.Buttons & PSP_CTRL_CROSS)
						{
						if(0 == reloadCount && pObject->position.z > -TUBE_LENGTH)
							{
							// fire bullet
							g_shotsFired++;
							WFPlaySound(0, SAMPLE_PLAYERFIRE);
							Vector pos = Scene.GetObject(PLAYER_LIST, PLAYER1_ID)->position;
							pos.z -= 4.0f; //(Scene.GetObject(PLAYER1_ID)->x, Scene.GetObject(PLAYER1_ID)->y, Scene.GetObject(PLAYER1_ID)->z - 4.0f);
							Vector vel(0.0, 0.0, -3.0);
							FireBullet(pos, vel);
							// firing uses charge energy (unless charged up!)
							if(g_charge > 0 && g_charge < CHARGE_FULL)
								g_charge--;
							// if charged up, then rapid fire
							if(g_charge < CHARGE_FULL)
								reloadCount = RELOAD_COUNT;
							else
								reloadCount = (unsigned int)(RELOAD_COUNT*0.66);
							}
						}

					// check bomb button
					if(pad.Buttons & PSP_CTRL_TRIANGLE)
						{
						if(CHARGE_FULL == g_charge)
							{
							// fire smart bomb
							WFPlaySound(2, SAMPLE_BOMB);
							FireSmartBomb();
							g_charge = 0;
							// flash screen
							Scene.SetBackColour(1.0f, 0.0f, 0.0f);
							}
						}

					// check for level skip (up to level 9)
					// TODO: Allow warp if charge full
					if((pad.Buttons & PSP_CTRL_RTRIGGER) && frames > 100 && level < 9)
						{
						if(0 == reloadCount)
							{
							runState = RUNSTATE_LEVELCOMPLETE;
							}
						}
					} // end if process player

				// Check if pause pressed
				if(pad.Buttons & PSP_CTRL_START)
					DoPause();

//				// Check if the ESC key was pressed
//				if(pad.Buttons & PSP_CTRL_START)
//					runState = RUNSTATE_QUIT;

				// check for player hit
				if(IsPlayerHit() && 0 == playerDying)
					{
					WFPlaySound(3, SAMPLE_PLAYEREXPLODE);
					// set player to "dying" state
					E3D_Object* player = Scene.GetObject(PLAYER_LIST, PLAYER1_ID);
					Vector v(0.0f, 0.0f,0.0f);
					AddFrags(player->position, v, 50);
					player->SetPosition(OFFSCREEN);
					player->behaviour = BEHAVE_NONE;		// switch off enemy
					playerDying = 200;						// player dying counter
					g_charge = 0;							// take away charge accumulated
					g_lives--;								// take away a life
					g_shakeCount = 30;
					}

				// update player "dying" counter
				if(playerDying > 0)	playerDying--;
				if(1 == playerDying) runState = RUNSTATE_PLAYERKILLED;

				// exit level
				if(IsLevelComplete())
					{
   					pObject = Scene.GetObject(PLAYER_LIST, PLAYER1_ID);
					pObject->position.z -= 1.5f;
					if(pObject->position.z < (-TUBE_LENGTH * 3) )
                        runState = RUNSTATE_LEVELCOMPLETE;
					}

				if(reloadCount) reloadCount--;
				} // wend running
			
			// check for reason play loop is finished
			switch(runState)
				{
				case RUNSTATE_QUIT :
					// Exit back to PSP OS
					ShutdownSound();
					sceGuTerm();
					sceKernelExitGame();
					return 0;									// Exit The Program
					break;
				case RUNSTATE_LEVELCOMPLETE :
					level++;
					//printf("Level up: %d\n", level);
					break;
				case RUNSTATE_PLAYERKILLED :
					{
					if(0 == g_lives)
						gameOver = true;
					// Player killed going down tube?
					if (IsLevelComplete())
						level++;
					}
					break;
				}
			} // wend !gameOver

		// Enter high-score if neccessary
		DoHiscoreEntry(g_score, level);
		} // wend (1)

    // Exit back to PSP OS
	ShutdownSound();
	sceGuTerm();
	sceKernelExitGame();
	return 0;									// Exit The Program
}

